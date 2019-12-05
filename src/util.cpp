#include <arg/util.hpp>

namespace arg::util {

bool startsWith(const std::string& string, const std::string& prefix)
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

std::string removePrefix(const std::string& string, const std::string& prefix)
{
    return string.substr(prefix.size());
}

KeyValue splitKeyValue(
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

std::vector<std::string> splitFlagMerge(
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
