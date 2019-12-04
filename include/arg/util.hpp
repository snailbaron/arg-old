#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace arg::util {

template <class T>
T parseValue(const std::string& string)
{
    // TODO: parse strings with spaces
    T value;
    std::istringstream{string} >> value;
    return value;
}

bool startsWith(const std::string& string, const std::string& prefix);
std::string removePrefix(const std::string& string, const std::string& prefix);

struct KeyValue {
    operator bool() const
    {
        return !key.empty() && !value.empty();
    }

    std::string key = "";
    std::string value = "";
};

KeyValue splitKeyValue(
    const std::string keyValueSeparator, const std::string& string);

std::string join(
    const std::vector<std::string>& strings, const std::string& separator);

template <class... Ts>
std::string join(const std::string& separator, Ts&&... args)
{
    std::vector<std::string> strings{std::forward<Ts>(args)...};
    std::ostringstream output;
    if (!strings.empty()) {
        output << strings.front();
        for (size_t i = 1; i < strings.size(); i++) {
            output << separator << strings.at(i);
        }
    }
    return output.str();
}

} // namespace arg::util
