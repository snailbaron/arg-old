#pragma once

#include <arg/argument_stream.hpp>
#include <arg/arguments.hpp>
#include <arg/errors.hpp>
#include <arg/util.hpp>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace arg {

class Parser {
public:
    template <class... Flags>
    Value<bool> flag(Flags&&... flags)
    {
        return addKeyArgument<Value<bool>>(std::forward<Flags>(flags)...);
    }

    template <class... Flags>
    MultiFlag multiFlag(Flags&&... args)
    {
        return addKeyArgument<MultiFlag>(std::forward<Flags>(args)...);
    }

    template <class T = std::string, class... Flags>
    Value<T> option(Flags&&... args)
    {
        return addKeyArgument<Value<T>>(std::forward<Flags>(args)...);
    }

    template <class T = std::string, class... Flags>
    MultiValue<T> multiOption(Flags&&... args)
    {
        return addKeyArgument<MultiValue<T>>(std::forward<Flags>(args)...);
    }

    template <class T = std::string>
    Value<T> argument()
    {
        auto valueData = std::make_shared<typename Value<T>::Data>();
        _positionalData.push_back(valueData);
        return {valueData};
    }

    template <class T = std::string, class... Ts>
    MultiValue<T> multiArgument()
    {
        if (_captor) {
            _err << "multi-argument is created multiple times\n";
        }
        auto ptr = std::make_shared<typename MultiValue<T>::Data>();
        _captor = ptr;
        return {ptr};
    }

    template <class... Flags>
    void help(Flags&&... flags)
    {
        static_assert(
            std::conjunction<std::is_convertible<Flags, std::string>...>(),
            "all help flags must be convertible to strings");

        if (!_helpFlags.empty()) {
            _err << "help flags are set multiple times\n";
        }
        _helpFlags.clear();
        ((_helpFlags.insert(std::forward<Flags>(flags))), ...);
    }

    void parse(const std::vector<std::string>& args);

    template <class Container = std::initializer_list<std::string>>
    void parse(const Container& args)
    {
        parse(ArgumentStream(args));
    }

    void parse(int argc, char* argv[])
    {
        std::vector<std::string> args;
        for (int i = 1; i < argc; i++) {
            args.push_back(argv[i]);
        }
        parse(std::move(args));
    }

private:
    template <class ArgumentType, class... Flags>
    ArgumentType addKeyArgument(Flags&&... flags)
    {
        static_assert(
            std::conjunction<std::is_convertible<Flags, std::string>...>(),
            "all flags must be convertible to strings");
        std::vector<std::string> flagVector{std::forward<Flags>(flags)...};

        auto data = std::make_shared<typename ArgumentType::Data>();
        data->fullName = util::join(", ", flagVector);
        data->shortName = flagVector.front();

        _keyDataOrdered.push_back(data);

        for (const auto& flag : flagVector) {
            if (_keyData.count(flag)) {
                _err << "flag '" << flag << "' is used multiple times\n";
            }
            _keyData[flag] = data;
        }

        return {data};
    }

    void printUsage() const
    {
        auto printOption = [this] (const auto& option) {
            *_out << option->shortName;
            if (option->needsArguments()) {
                *_out << " " << option->metavar;
            }
        };

        *_out << "Usage: " << _programName;
        for (const auto& option : _keyDataOrdered) {
            *_out << " ";
            if (!option->required) {
                *_out << "[";
            }
            printOption(option);
            if (option->multi()) {
                *_out << " [";
                printOption(option);
                *_out << "]";
            }
            if (!option->required) {
                *_out << "]";
            }
        }

        for (const auto& arg : _positionalData) {
            *_out << " ";
            if (arg->required) {
                *_out << arg->metavar;
            } else {
                *_out << "[" << arg->metavar << "]";
            }
        }

        if (_captor) {
            *_out << " [" << _captor->metavar << " [" << _captor->metavar <<
                "... ] ]";
        }

        *_out << "\n";

        size_t maxNameLength = 0;
        for (const auto& option : _keyDataOrdered) {
            if (option->fullName.size() > maxNameLength) {
                maxNameLength = option->fullName.size();
            }
        }
        for (const auto& arg : _positionalData) {
            if (arg->fullName.size() > maxNameLength) {
                maxNameLength = arg->fullName.size();
            }
        }

        for (const auto& option : _keyDataOrdered) {
            *_out << "  " <<
                std::left << std::setw(maxNameLength) << option->fullName <<
                "  " << option->help << "\n";
        }
        for (const auto& arg : _positionalData) {
            *_out << "  " <<
                std::left << std::setw(maxNameLength) << arg->metavar <<
                "  " << arg->help << "\n";
        }
        if (_captor) {
            *_out << "  " <<
                std::left << std::setw(maxNameLength) << _captor->metavar <<
                "  " << _captor->help << "\n";
        }
    }

    bool isFlagMerge(const std::vector<std::string>& flags) const
    {
        if (flags.empty()) {
            return false;
        }

        for (size_t i = 0; i + 1 < flags.size(); i++) {
            if (!_keyData.count(flags[i]) ||
                    _keyData.at(flags[i])->needsArguments()) {
                return false;
            }
        }

        if (!_keyData.count(flags.back())) {
            return false;
        }

        return true;
    }

    std::ostream* _out = &std::cout;
    Errors _err{std::cerr};

    std::string _programName = "program";
    std::string _flagPrefix = "-";
    std::string _keyValueSeparator = "=";

    std::set<std::string> _helpFlags;
    std::vector<std::shared_ptr<ArgumentData>> _keyDataOrdered;
    std::map<std::string, std::shared_ptr<ArgumentData>> _keyData;
    std::vector<std::shared_ptr<ArgumentData>> _positionalData;
    size_t _position = 0;
    std::shared_ptr<ArgumentData> _captor;
    std::vector<std::string> _leftovers;
};

Parser globalParser;

template <class... Ts>
Value<bool> flag(Ts&&... args)
{
    return globalParser.flag(std::forward<Ts>(args)...);
}

template <class... Ts>
MultiFlag multiFlag(Ts&&... args)
{
    return globalParser.multiFlag(std::forward<Ts>(args)...);
}

template <class T = std::string, class... Ts>
Value<T> option(Ts&&... args)
{
    return globalParser.option<T>(std::forward<Ts>(args)...);
}

template <class T = std::string, class... Ts>
MultiValue<T> multiOption(Ts&&... args)
{
    return globalParser.multiOption<T>(std::forward<Ts>(args)...);
}

template <class T = std::string, class... Ts>
Value<T> argument(Ts&&... args)
{
    return globalParser.argument<T>(std::forward<Ts>(args)...);
}

template <class T = std::string, class... Ts>
MultiValue<T> multiArgument(Ts&&... args)
{
    return globalParser.multiArgument<T>(std::forward<Ts>(args)...);
}

template <class... Flags>
void help(Flags&&... flags)
{
    globalParser.help(std::forward<Flags>(flags)...);
}

void parse(int argc, char* argv[])
{
    globalParser.parse(argc, argv);
}

} // namespace arg
