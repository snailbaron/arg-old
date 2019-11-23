#pragma once

#include <car/flag.hpp>
#include <car/value.hpp>
#include <car/multivalue.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace car {

namespace {

} // namespace

class Parser {
public:
    template <class... Args>
    Flag flag(const Args&... args)
    {
        auto flagData = std::make_shared<FlagData>();
        ((_flags[args] = flagData), ...);
        return Flag{std::move(flagData)};
    }

    template <class... Args>
    MultiFlag multiFlag(const Args&... args)
    {
        auto multiFlagData = std::make_shared<MultiFlagData>();
        ((_flags[args] = multiFlagData), ...);
        return MultiFlag{std::move(multiFlagData)};
    }

    template <class T = std::string, class... Args>
    Value<T> option(const Args&... args)
    {
        auto valueData = std::make_shared<ValueData<T>>();
        ((_options[args] = valueData), ...);
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

    bool parse(int argc, char* argv[]);
    bool parse(const std::vector<std::string>& args);

    void printUsage() const;

private:
    bool isValidFlagMerge(const std::string& flagMerge);

    std::ostream& _err = std::cerr;

    std::string _programName;
    const std::string _flagPrefix = "-";

    std::map<std::string, std::weak_ptr<Flag>> _flags;
    std::map<std::string, std::weak_ptr<AbstractValue>> _options;
    std::vector<std::weak_ptr<AbstractValue>> _arguments;
    size_t _argumentsRead = 0;
    std::vector<std::string> _leftovers;
};

} // namespace car
