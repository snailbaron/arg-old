#pragma once

#include <deque>
#include <string>
#include <utility>

class ArgumentStream {
public:
    std::string next()
    {
        std::string first = std::move(_args.front());
        _args.pop_front();
        return first;
    }

    bool empty() const
    {
        return _args.empty();
    }

private:
    std::deque<std::string> _args;
};
