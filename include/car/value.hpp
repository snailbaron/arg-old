#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace car {

// TODO: Add default value. Check if option type is default-constructible where
// required.

struct AbstractValueData {
    virtual void set(const std::string& string) = 0;
};

template <class T>
struct ValueData : AbstractValueData {
    void set(const std::string& string) override
    {
        // TODO: check for duplicate set?
        std::istringstream{string} >> value;
    }

    T value;
};

template <class T>
struct MultiValueData : AbstractValueData {
    void set(const std::string& string) override
    {
        T value;
        std::istringstream{string} >> value;
        values.push_back(std::move(value));
    }

    std::vector<T> values;
};

template <class T>
class Value {
public:
    Value(std::shared_ptr<ValueData<T>>&& data)
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

private:
    std::shared_ptr<ValueData<T>> _data;
};

template <class T>
class MultiValue {
public:
    MultiValue(std::shared_ptr<MultiValueData<T>>&& data)
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

private:
    std::shared_ptr<MultiValueData<T>> _data;
};

} // namespace car
