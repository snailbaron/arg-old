#include <arg/parser.hpp>
#include <arg/util.hpp>

#include <cassert>
#include <string_view>
#include <vector>

namespace arg {

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

bool Parser::raiseFlag(const std::string& flag)
{
    auto flagIt = _flagData.find(flag);
    if (flagIt != _flagData.end()) {
        auto& flagData = *flagIt->second;
        if (flagData.value) {
            // TODO
            _err << "flag set multiple times: " << "\n";
        }
        flagData.value = true;
        return true;
    }

    auto multiFlagIt = _multiFlagData.find(flag);
    if (multiFlagIt != _multiFlagData.end()) {
        auto& multiFlagData = *multiFlagIt->second;
        multiFlagData.count++;
        return true;
    }

    return false;
}

bool Parser::setOption(const std::string& flag, ArgumentStream& args)
{
    auto optionIt = _optionData.find(flag);
    if (optionIt != _optionData.end()) {
        auto& optionData = *optionIt->second;
        if (optionData.value) {
            // TODO
            _err << "option set multiple times: " << "\n";
        }
        optionData.set(args.next());
        return true;
    }

    auto multiOptionIt = _multiOptionData.find(flag);
    if (multiOptionIt != _multiOptionData.end()) {
        auto& multiOptionData = *multiOptionIt->second;
        multiOptionData.add(args.next());
        return true;
    }

    return false;
}

bool Parser::parse(ArgumentStream& args)
{
    for (auto arg = args.next(); !args.empty(); arg = args.next()) {
        if (raiseFlag(arg) || setOption(arg, args) {
            continue;
        }

        auto unmergedFlags = splitFlagMerge(_flagPrefix, arg);
        if (isFlagMerge(unmergedFlags)) {
            for (size_t i = 0; i + 1 < unmergedFlags.size(); i++) {
                raiseFlag(unmergedFlags[i]);
            }

            const auto& lastFlag = unmergedFlags.back();
            raiseFlag(lastFlag) || setOption(lastFlag, args);

            continue;
        }

        auto keyValue = splitKeyValue(_keyValueSeparator, arg);
        if (keyValue && setOption(arg, args)) {
            continue;
        }

        if (_position < _positionalData.size()) {
            _positionalData.at(_position++)->set(arg);
            continue;
        }

        if (_captor) {
            _captor->add(arg);
            continue;
        }

        _leftovers.push_back(std::move(arg));
    }
}

bool Parser::isFlagMerge(const std::vector<std::string>& flags)
{
    if (flags.empty()) {
        return false;
    }

    for (size_t i = 0; i + 1 < flags.size(); i++) {
        if (!_flagData.count(flags[i]) && !_multiFlagData.count(flags[i])) {
            return false;
        }
    }

    if (!_flagData.count(flags.back()) &&
            !_optionData.count(flags.back()) &&
            !_optionData.count(flags.back()) &&
            !_multiOptionData.count(flags.back())) {
        return false;
    }

    return true;
}

} // namespace arg
