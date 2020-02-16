#include <arg/parser.hpp>

namespace arg {

using ArgIterator = std::vector<std::string>::iterator;
using ArgConstIterator = std::vector<std::string>::const_iterator;

void Parser::parseValue(
    Argument& argument, ArgConstIterator& it, ArgConstIterator end)
{
    if (argument.wasExplicitlySet() && !argument.multi()) {
        // TODO: warn: a non-multi value set multiple times; only the
        // last value is used
    }

    if (argument.expectsValue()) {
        ++it;
        if (it != end) {
            argument.parse(*it);
        } else {
            // TODO: no value for a value-requiring argument
        }
    } else {
        argument.set();
    }
}

void Parser::parse(const std::vector<std::string>& args)
{
    for (auto arg = args.begin(); arg != args.end(); ++arg) {
        if (_helpFlags.count(*arg)) {
            printUsage();
            std::exit(0);
        }

        if (auto it = _keyData.find(args.front()); it != _keyData.end()) {
            parseValue(it->second, arg, args.end());
            continue;
        }

        auto flags = util::splitFlagMerge(_flagPrefix, args.front());
        if (isFlagMerge(flags)) {
            for (const auto& flag : flags) {
                parseValue(_keyData.at(flag), arg, args.end());
            }
            continue;
        }

        auto keyValue = util::splitKeyValue(_keyValueSeparator, args.front());
        if (keyValue) {
            if (auto it = _keyData.find(keyValue.key); it != _keyData.end()) {
                auto& argument = it->second;
                if (argument->needsArguments()) {
                    argument->parse(keyValue.argument);
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

        _leftovers.push_back(*arg);
    }

    if (_err) {
        std::exit(1);
    }
}

} // namespace arg
