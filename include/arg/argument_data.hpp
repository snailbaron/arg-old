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
    virtual void parse(ArgumentStream&, std::ostream& error) = 0;
    virtual bool needsArguments() const = 0;

    std::string name;
    std::string help;
    std::string metavar;
};

struct FlagData final : ArgumentData {
    void parse(ArgumentStream&, std::ostream& error) override
    {
        if (value) {
            error << "flag set multiple times: " << name << "\n";
        }
        value = true;
    }

    bool needsArguments() const override
    {
        return false;
    }

    bool value = false;
};

struct MultiFlagData final : ArgumentData {
    void parse(ArgumentStream&, std::ostream&) override
    {
        count++;
    }

    bool needsArguments() const override
    {
        return false;
    }

    size_t count = 0;
};

template <class T>
struct ValueData final : ArgumentData {
    void parse(ArgumentStream& stream, std::ostream& error) override
    {
        if (stream.empty()) {
            error << "no value for argument: " << name << "\n";
            return;
        }
        value = util::parseValue<T>(stream.pop());
    }

    bool needsArguments() const override
    {
        return true;
    }

    bool required = false;
    std::optional<T> value;
};

template <class T>
struct MultiValueData final : ArgumentData {
    void parse(ArgumentStream& stream, std::ostream& error) override
    {
        if (stream.empty()) {
            error << "no value for argument: " << name << "\n";
            return;
        }
        values.push_back(util::parseValue<T>(stream.pop()));
    }

    bool needsArguments() const override
    {
        return true;
    }

    std::vector<T> values;
};

} // namespace arg
