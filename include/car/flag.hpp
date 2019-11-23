#pragma once

#include <memory>
#include <utility>

namespace car {

struct AbstractFlagData {
    virtual void raise() = 0;
};

struct FlagData : AbstractFlagData {
    void raise() override
    {
        // TODO: check for duplicate raise?
        value = true;
    }

    bool value = false;
};

struct MultiFlagData : AbstractFlagData {
    void raise() override
    {
        count++;
    }

    size_t count = 0;
};

class Flag {
public:
    Flag(std::shared_ptr<FlagData>&& data)
        : _data(std::move(data))
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
    MultiFlag(std::shared_ptr<MultiFlagData>&& data)
        : _data(std::move(data))
    { }

private:
    std::shared_ptr<MultiFlagData> _data;
};

} // namespace
