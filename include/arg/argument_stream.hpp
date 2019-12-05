#pragma once

#include <string>
#include <utility>
#include <vector>

class ArgumentStream {
public:
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
