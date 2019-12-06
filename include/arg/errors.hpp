#pragma once

#include <ostream>

namespace arg {

class Errors {
public:
    Errors(std::ostream& output)
        : _output(&output)
    { }

    template <class T>
    Errors& operator<<(const T& value)
    {
        _written = true;
        *_output << value;
        return *this;
    }

    operator bool() const
    {
        return _written;
    }

    void stream(std::ostream& s)
    {
        _output = &s;
    }

private:
    std::ostream* _output;
    bool _written = false;
};

} // namespace arg
