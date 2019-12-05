#include <arg/parser.hpp>
#include <arg/util.hpp>

#include <cassert>
#include <string_view>
#include <vector>

namespace arg {

bool Parser::parse(ArgumentStream& args)
{
    while (!args.empty()) {
        if (auto it = _keyData.find(args.front()); it != _keyData.end()) {
            it->second->parse(args.shift(), _err);
            continue;
        }

        auto unmergedFlags = util::splitFlagMerge(_flagPrefix, args.front());
        if (isFlagMerge(unmergedFlags)) {
            args.shift();
            for (const auto& flag : unmergedFlags) {
                _keyData.at(flag)->parse(args, _err);
            }
            continue;
        }

        auto keyValue = util::splitKeyValue(_keyValueSeparator, args.front());
        if (keyValue) {
            if (auto it = _keyData.find(keyValue.key); it != _keyData.end()) {
                if (it->second->needsArguments()) {
                    args.replace(std::move(keyValue.value));
                    it->second->parse(args, _err);
                    continue;
                }
            }
        }

        if (_position < _positionalData.size()) {
            _positionalData.at(_position++)->parse(args, _err);
            continue;
        }

        if (_captor) {
            _captor->parse(args, _err);
            continue;
        }

        _leftovers.push_back(args.pop());
    }
}

bool Parser::isFlagMerge(const std::vector<std::string>& flags) const
{
    if (flags.empty()) {
        return false;
    }

    for (size_t i = 0; i + 1 < flags.size(); i++) {
        if (!_keyData.count(flags[i]) ||
                _keyData.at(flags[i])->needsArguments()) {
            return false;
        }
    }

    if (!_keyData.count(flags.back())) {
        return false;
    }

    return true;
}

} // namespace arg
