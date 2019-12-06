#pragma once

#include <arg/argument_stream.hpp>
#include <arg/arguments.hpp>
#include <arg/errors.hpp>
#include <arg/util.hpp>

#include <cstdlib>
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
    Flag flag(Flags&&... flags)
    {
        return addKeyArgument<Flag>(std::forward<Flags>(flags)...);
    }

    template <class... Flags>
    MultiFlag multiFlag(const Flags&... args)
    {
        return addArgumentData<MultiFlag>(std::forward<Flags>(args)...);
    }

    template <class T = std::string, class... Flags>
    Value<T> option(const Flags&... args)
    {
        return addArgumentData<Value<T>>(std::forward<Flags>(args)...);
    }

    template <class T = std::string, class... Flags>
    MultiValue<T> multiOption(const Flags&... args)
    {
        return addArgumentData<MultiValue<T>>(std::forward<Flags>(args)...);
    }

    template <class T = std::string>
    Value<T> argument()
    {
        auto valueData = std::make_shared<Value<T>::Data>();
        _positionalData.push_back(valueData);
        return {valueData};
    }

    template <class T = std::string, class... Ts>
    MultiValue<T> multiArgument()
    {
        if (_captor) {
            _err << "multi-argument is created multiple times\n";
        }
        _captor = std::make_shared<MultiValue<T>::Data>();
        return {_captor};
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

    void parse(std::vector<std::string> args)
    {
        parse(ArgumentStream(std::move(args)));
    }

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

        auto data = std::make_shared<ArgumentType::Data>();
        data->name = util::join(", ", flagVector);
        for (const auto& flag : flagVector) {
            if (_keyData.count(flag)) {
                _err << "flag '" << flag << "' is used multiple times\n";
            }
            _keyData[flag] = data;
            _keyDataOrdered.push_back(data);
        }
        return {data};
    }

    void printUsage() const
    {
        auto printOption = [this] (const auto& option) {
            *_out << option->name;
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

        for (const auto& option : _keyDataOrdered) {
            *_out << option->name << " : " << option->help << "\n";
        }
        for (const auto& arg : _positionalData) {
            *_out << arg->name << " : " << arg->help << "\n";
        }
        if (_captor) {
            *_out << _captor->name << " : " << _captor->help << "\n";
        }
    }

    void parse(ArgumentStream args)
    {
        // TODO: check that there are no required values after optional at the
        // moment of parsing

        while (!args.empty()) {
            if (_helpFlags.count(args.front())) {
                printUsage();
                std::exit(0);
            }

            if (auto it = _keyData.find(args.front()); it != _keyData.end()) {
                it->second->parse(args.shift(), _err);
                continue;
            }

            auto unmergedFlags = util::splitFlagMerge(_flagPrefix, args.front());
            if (isFlagMerge(unmergedFlags)) {
                args.shift();
                for (const auto& flag : unmergedFlags) {
                    _keyData.at(flag)->parse(args, _err);
                }
                continue;
            }

            auto keyValue = util::splitKeyValue(_keyValueSeparator, args.front());
            if (keyValue) {
                if (auto it = _keyData.find(keyValue.key); it != _keyData.end()) {
                    if (it->second->needsArguments()) {
                        args.replace(std::move(keyValue.value));
                        it->second->parse(args, _err);
                        continue;
                    }
                }
            }

            if (_position < _positionalData.size()) {
                _positionalData.at(_position++)->parse(args, _err);
                continue;
            }

            if (_captor) {
                _captor->parse(args, _err);
                continue;
            }

            _leftovers.push_back(args.pop());
        }

        if (_err) {
            std::exit(1);
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

extern Parser globalParser;

template <class... Ts>
Flag flag(Ts&&... args)
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
Value<T> multiArgument(Ts&&... args)
{
    return globalParser.multiArgument<T>(std::forward<Ts>(args)...);
}

void parse(int argc, char* argv[])
{
    globalParser.parse(argc, argv);
}

} // namespace arg
