#pragma once

#include <sstream>
#include <string>
#include <string_view>
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

inline bool startsWith(const std::string& string, const std::string& prefix)
{
    if (string.length() < prefix.length()) {
        return false;
    }

    for (size_t i = 0; i < prefix.length(); i++) {
        if (string[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}

inline std::string removePrefix(
    const std::string& string, const std::string& prefix)
{
    return string.substr(prefix.size());
}

struct KeyValue {
    operator bool() const
    {
        return !key.empty() && !value.empty();
    }

    std::string key = "";
    std::string value = "";
};

inline KeyValue splitKeyValue(
    const std::string keyValueSeparator, const std::string& string)
{
    auto separatorPosition = string.find(keyValueSeparator);
    if (separatorPosition != std::string::npos) {
        return {
            string.substr(0, separatorPosition),
            string.substr(separatorPosition + keyValueSeparator.length())
        };
    } else {
        return {};
    }
}

inline std::string join(
    const std::string& separator, const std::vector<std::string>& strings)
{
    auto result = std::string{};
    if (!strings.empty()) {
        result += strings.front();
        for (size_t i = 1; i < strings.size(); i++) {
            result += separator + strings.at(i);
        }
    }
    return result;
}

inline std::vector<std::string> splitFlagMerge(
    const std::string& flagMerge, const std::string& flagPrefix)
{
    if (util::startsWith(flagMerge, flagPrefix)) {
        auto flagString = util::removePrefix(flagMerge, flagPrefix);
        std::vector<std::string> flags;
        for (const auto& flagLetter : flagString) {
            flags.push_back(flagPrefix + flagLetter);
        }
        return flags;
    } else {
        return {};
    }
}

} // namespace arg::util
