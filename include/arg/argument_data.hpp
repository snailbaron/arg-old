#pragma once

#include <arg/argument_stream.hpp>
#include <arg/util.hpp>

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <optional>

namespace arg {

struct ArgumentData {
    virtual ~ArgumentData() {}

    std::string name;
    std::string help;
    std::string metavar;
};

struct FlagData final : ArgumentData {
    bool value = false;
    std::string help;
};

struct MultiFlagData final : ArgumentData {
    size_t count = 0;
    std::string help;
};

struct ValueData : ArgumentData {
    virtual ~ValueData() {}
    virtual void set(const std::string& valueString) = 0;

    bool required = false;
    std::string help;
};

template <class T>
struct TypedValueData final : ValueData {
    void set(const std::string& valueString) override
    {
        value = util::parseValue<T>(valueString);
    }

    std::optional<T> value;
};

struct MultiValueData : ArgumentData {
    virtual void add(const std::string& valueString) = 0;

    std::string help;
};

template <class T>
struct TypedMultiValueData final : MultiValueData {
    void add(const std::string& valueString) override
    {
        values.push_back(util::parseValue<T>(valueString));
    }

    std::vector<T> values;
};

} // namespace arg
