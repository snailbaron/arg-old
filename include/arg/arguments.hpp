#pragma once

#include <arg/data.hpp>

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace arg {

class Flag {
public:
    Flag(const std::shared_ptr<FlagData>& data)
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

private:
    std::shared_ptr<FlagData> _data;
};

class MultiFlag {
public:
    MultiFlag(const std::shared_ptr<MultiFlagData>& data)
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

private:
    std::shared_ptr<MultiFlagData> _data;
};

template <class T>
class Value {
public:
    Value(std::shared_ptr<TypedValueData<T>>&& data)
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

private:
    std::shared_ptr<TypedValueData<T>> _data;
};

template <class T>
class MultiValue {
public:
    MultiValue(std::shared_ptr<TypedMultiValueData<T>>&& data)
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

    MultiValue required()
    {
        _data->required = true;
        return *this;
    }

private:
    std::shared_ptr<TypedMultiValueData<T>> _data;
};

} // namespace arg
