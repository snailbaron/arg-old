#pragma once

#include <arg/errors.hpp>
#include <arg/util.hpp>

#include <memory>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace arg {

template <class T>
T parseValue(const std::string& string)
{
    // TODO: parse strings with spaces
    T value;
    std::istringstream{string} >> value;
    return value;
}

class ArgumentBase {
public:
    virtual ~ArgumentBase() {}

    virtual std::string_view help() const = 0;
    virtual void help(std::string_view helpString) = 0;

    virtual std::string_view metavar() const = 0;
    virtual void metavar(std::string_view metavarString) = 0;

    virtual bool required() const = 0;
    virtual void required(bool isRequired) = 0;

    virtual void raise() = 0;
    virtual void parse(std::string_view value) = 0;
};

template <class T, bool Multi, bool Flag>
class Argument : public ArgumentBase {
public:
    const auto& keys() const
    {
        return _data->keys;
    }

    void addKey(std::string key)
    {
        _data->keys.push_back(key);
    }

    std::string_view help() const override
    {
        return _data->help;
    }

    void help(std::string_view helpString) override
    {
        _data->help = helpString;
    }

    std::string_view metavar() const override
    {
        return _data->metavar;
    }

    void metavar(std::string_view metavarString) override
    {
        _data->metavar = metavarString;
    }

    bool required() const override
    {
        return _data->required;
    }

    void required(bool isRequired) override
    {
        _data->required = isRequired;
    }

    void raise() override
    {
        if constexpr (Flag) {
            if constexpr (Multi) {
                _data->values++;
            } else {
                _data->values = true;
            }
        } else {
            // TODO: report error
        }
    }

    void parse(std::string_view value) override
    {
        if constexpr (!Flag) {
            if constexpr (Multi) {
                _data->values.push_back(parseValue<T>(std::string{value}));
            } else {
                _data->values = parseValue<T>(std::string{value});
            }
        } else {
            // TODO: report error
        }
    }

    template <bool U = Multi, class = std::enable_if_t<!U>>
    const T& operator*() const
    {
        return *_data->values;
    }

    template <bool U = Multi, class = std::enable_if_t<!U>>
    operator const T&() const
    {
        return *this;
    }

    template <bool U = Multi, class = std::enable_if_t<U>>
    auto begin() const noexcept
    {
        return _data->values.begin();
    }

    template <bool U = Multi, class = std::enable_if_t<U>>
    auto begin() noexcept
    {
        return _data->values.begin();
    }

    template <bool U = Multi, class = std::enable_if_t<U>>
    auto end() const noexcept
    {
        return _data->values.end();
    }

    template <bool U = Multi, class = std::enable_if_t<U>>
    auto end() noexcept
    {
        return _data->values.end();
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help = "help message";
        std::string metavar = "VALUE";
        bool required = false;
        std::conditional_t<Multi, std::vector<T>, std::optional<T>> values;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

} // namespace arg
