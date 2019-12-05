#pragma once

#include <arg/argument_data.hpp>
#include <arg/argument_stream.hpp>
#include <arg/arguments.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <tuple>
#include <variant>
#include <type_traits>

namespace arg {

class Parser {
public:
    template <class... Ts>
    Flag flag(Ts&&... args)
    {
        return addArgumentData<Flag, FlagData>(std::forward<Ts>(args)...);
    }

    template <class... Ts>
    MultiFlag multiFlag(const Ts&... args)
    {
        return addArgumentData<MultiFlag, MultiFlagData>(
            std::forward<Ts>(args)...);
    }

    template <class T = std::string, class... Ts>
    Value<T> option(const Ts&... args)
    {
        return addArgumentData<Value<T>, ValueData<T>>(
            std::forward<Ts>(args)...);
    }

    template <class T = std::string, class... Ts>
    MultiValue<T> multiOption(const Ts&... args)
    {
        return addArgumentData<MultiValue<T>, ValueData<T>>(
            std::forward<Ts>(args)...);
    }

    template <class T = std::string>
    Value<T> argument()
    {
        // TODO: check that there are no required values after optional at the
        // moment of parsing

        auto valueData = std::make_shared<ValueData<T>>();
        _positionalData.push_back(valueData);
        return {valueData};
    }

    template <class T = std::string, class... Ts>
    MultiValue<T> multiArgument(const Ts&... args)
    {
        // TODO: check that there is no more than one multi-argument

        _captor = std::make_shared<MultiValueData<T>>();
        return {_captor};
    }

    void parse(int argc, char* argv[]);

private:
    template <class ArgumentType, class DataType, class... Ts>
    ArgumentType addArgumentData(Ts&&... args)
    {
        static_assert(
            std::conjunction<std::is_same<Ts, std::string>...>(),
            "all flags must be strings");

        auto data = std::make_shared<DataType>();
        // TODO: check for duplicates in args
        // TODO: check for duplicates with other keys
        data->name = util::join(", ", std::forward<Ts>(args)...);
        ((_keyData[args] = data), ...);

        return {data};
    }

    bool parse(ArgumentStream& args);
    bool isFlagMerge(const std::vector<std::string>& flags) const;

    std::ostream& _err = std::cerr;

    std::string _programName;
    std::string _flagPrefix = "-";
    std::string _keyValueSeparator = "=";

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
