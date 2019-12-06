#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

class ArgumentStream {
public:
    ArgumentStream(std::vector<std::string> args)
        : _args(std::move(args))
    {
        std::reverse(_args.begin(), _args.end());
    }

    template <class Container = std::initializer_list<std::string>>
    ArgumentStream(const Container& container)
    {
        std::move(
            container.begin(), container.end(), std::back_inserter(_args));
        std::reverse(_args.begin(), _args.end());
    }

    const std::string& front() const
    {
        return _args.back();
    }

    void push(std::string arg)
    {
        _args.push_back(std::move(arg));
    }

    std::string pop()
    {
        std::string arg = std::move(_args.back());
        _args.pop_back();
        return arg;
    }

    void replace(std::string arg)
    {
        _args.back() = std::move(arg);
    }

    ArgumentStream& shift()
    {
        _args.pop_back();
        return *this;
    }

    bool empty() const
    {
        return _args.empty();
    }

private:
    std::vector<std::string> _args;
};
