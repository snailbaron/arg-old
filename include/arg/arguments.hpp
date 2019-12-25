#pragma once

#include <arg/argument_stream.hpp>
#include <arg/errors.hpp>
#include <arg/util.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace arg {

struct ArgumentData {
    virtual ~ArgumentData() {}
    virtual void parse(ArgumentStream&, Errors& error) = 0;
    virtual bool needsArguments() const { return false; }
    virtual bool multi() const { return false; }

    std::string fullName;
    std::string shortName;
    std::string help;
    std::string metavar = "VALUE";
    bool required = false;
};

class Flag {
public:
    struct Data final : ArgumentData {
        void parse(ArgumentStream&, Errors& error) override
        {
            if (value) {
                error << "flag set multiple times: " << fullName << "\n";
            }
            value = true;
        }

        bool value = false;
    };

    Flag(const std::shared_ptr<Data>& data)
        : _data(data)
    { }

    bool operator*() const
    {
        return _data->value;
    }

    operator bool() const
    {
        return **this;
    }

    Flag help(std::string helpMessage)
    {
        _data->help = std::move(helpMessage);
        return *this;
    }

private:
    std::shared_ptr<Data> _data;
};

class MultiFlag {
public:
    struct Data final : ArgumentData {
        void parse(ArgumentStream&, Errors&) override
        {
            count++;
        }

        bool multi() const override {
            return true;
        }

        size_t count = 0;
    };

    MultiFlag(const std::shared_ptr<Data>& data)
        : _data(data)
    { }

    std::size_t operator*() const
    {
        return _data->count;
    }

    operator std::size_t() const
    {
        return **this;
    }

    MultiFlag help(std::string helpMessage)
    {
        _data->help = std::move(helpMessage);
        return *this;
    }

private:
    std::shared_ptr<Data> _data;
};

template <class T>
class Value {
public:
    struct Data final : ArgumentData {
        void parse(ArgumentStream& stream, Errors& error) override
        {
            if (stream.empty()) {
                error << "no value for argument: " << fullName << "\n";
                return;
            }
            value = util::parseValue<T>(stream.pop());
        }

        bool needsArguments() const override
        {
            return true;
        }

        std::optional<T> value;
    };

    Value(std::shared_ptr<Data> data)
        : _data(std::move(data))
    { }

    const T& operator*() const
    {
        return _data->value;
    }

    operator const T&() const
    {
        return **this;
    }

    Value required()
    {
        _data->required = true;
        return *this;
    }

    Value help(std::string helpMessage)
    {
        _data->help = std::move(helpMessage);
        return *this;
    }

private:
    std::shared_ptr<Data> _data;
};

template <class T>
class MultiValue {
public:
    struct Data final : ArgumentData {
        void parse(ArgumentStream& stream, Errors& error) override
        {
            if (stream.empty()) {
                error << "no value for argument: " << fullName << "\n";
                return;
            }
            values.push_back(util::parseValue<T>(stream.pop()));
        }

        bool needsArguments() const override
        {
            return true;
        }

        bool multi() const override
        {
            return true;
        }

        std::vector<T> values;
    };

    MultiValue(std::shared_ptr<Data> data)
        : _data(std::move(data))
    { }

    const std::vector<T>& operator*() const
    {
        return _data->values;
    }

    operator const std::vector<T>&() const
    {
        return **this;
    }

    MultiValue help(std::string helpMessage)
    {
        _data->help = std::move(helpMessage);
        return *this;
    }

private:
    std::shared_ptr<Data> _data;
};

} // namespace arg
