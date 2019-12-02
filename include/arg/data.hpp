#pragma once

#include <arg/argument_stream.hpp>

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <optional>

namespace arg {

template <class T>
T parseValue(const std::string& string)
{
    // TODO: parse strings with spaces
    T value;
    std::istringstream{string} >> value;
    return value;
}

struct FlagData {
    bool value = false;
    std::string help;
};

struct MultiFlagData {
    size_t count = 0;
    std::string help;
};

struct ValueData {
    virtual void set(const std::string& valueString) = 0;
    virtual bool isSet() const = 0;

    bool required = false;
    std::string help;
};

template <class T>
struct TypedValueData : ValueData {
    void set(const std::string& valueString) override
    {
        value = parseValue<T>(valueString);
    }

    bool isSet() const override
    {
        return value;
    }

    std::optional<T> value;
};

struct MultiValueData {
    virtual void add(const std::string& valueString) = 0;

    bool required = false;
    std::string help;
};

template <class T>
struct TypedMultiValueData : MultiValueData {
    void add(const std::string& valueString) override
    {
        values.push_back(parseValue<T>(valueString));
    }

    std::vector<T> values;
};

} // namespace arg
