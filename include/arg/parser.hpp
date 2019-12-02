#pragma once

#include <arg/argument_stream.hpp>
#include <arg/arguments.hpp>
#include <arg/data.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace arg {

std::string join(
    const std::vector<std::string>& strings, const std::string& separator)
{
    std::ostringstream output;
    if (!strings.empty()) {
        output << strings.front();
        for (size_t i = 1; i < strings.size(); i++) {
            output << separator << strings[i];
        }
    }
    return output.str();
}

class ArgumentStreamParser {
public:
    template <class... Args>
    Flag flag(const Args&... args)
    {
        std::vector<std::string> flags{args...};
        auto fullName = join(flags, ", ");
        auto flagData = std::make_shared<FlagData>();

        for (const auto& flag : flags) {
            addFullName(flag, fullName);
            _flagData[flag] = flagData;
        }

        return Flag{flagData};
    }

    template <class... Args>
    MultiFlag multiFlag(const Args&... args)
    {
        std::vector<std::string> flags{args...};
        auto fullName = join(flags, ", ");
        auto multiFlagData = std::make_shared<MultiFlagData>();

        for (const auto& flag : flags) {
            addFullName(flag, fullName);
            _multiFlagData[flag] = multiFlagData;
        }

        return MultiFlag{multiFlagData};
    }

    template <class T = std::string, class... Args>
    Value<T> option(const Args&... args)
    {
        std::vector<std::string> flags{args...};
        auto fullName = join(flags, ", ");
        auto valueData = std::make_shared<ValueData<T>>();

        for (const auto& flag : flags) {
            addFullName(flag, fullName);
            _optionData[flag] = valueData;
        }

        return Value{std::move(valueData)};
    }

    template <class T = std::string, class... Args>
    MultiValue<T> multiOption(const Args&... args)
    {
        auto multiValueData = std::make_shared<MultiValueData<T>>();
        ((_options[args] = multiValueData), ...);
        return MultiValue{std::move(multiValueData)};
    }

    template <class T = std::string, class... Args>
    Value<T> argument(const Args&... args)
    {
        auto valueData = std::make_shared<ValueData<T>>();

        ((_arguments[args] = valueData), ...);
        return Value{std::move(valueData)};
    }

    template <class T = std::string, class... Args>
    MultiValue<T> multiArgument(const Args&... args)
    {
        auto multiValueData = std::make_shared<MultiValueData<T>>();
        ((_arguments[args] = multiValueData), ...);
        return MultiValue{std::move(multiValueData)};
    }

    void parse(int argc, char* argv[]);

private:
    void addFullName(const std::string& flag, const std::string& fullName);
    bool isValidFlagMerge(const std::string& flagMerge);
    bool parseFlag(const std::string& arg);
    bool parseMultiFlag(const std::string& arg);

    std::ostream& _err = std::cerr;

    std::string _programName;
    const std::string _flagPrefix = "-";

    std::map<std::string, std::string> _fullNames;

    std::map<std::string, std::shared_ptr<FlagData>> _flagData;
    std::map<std::string, std::shared_ptr<MultiFlagData>> _multiFlagData;
    std::map<std::string, std::shared_ptr<ValueData>> _optionData;
    std::map<std::string, std::shared_ptr<MultiValueData>> _multiOptionData;
    std::vector<std::shared_ptr<ValueData>> _positionalData;
    size_t _position = 0;
    std::shared_ptr<MultiValueData> _captor;
    std::vector<std::string> _leftovers;
};

} // namespace arg
