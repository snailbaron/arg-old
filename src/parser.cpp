#include <car/parser.hpp>

#include <string_view>
#include <vector>

namespace car {

bool Parser::parse(int argc, char* argv[])
{
    if (argc > 0) {
        _programName = argv[0];
    }

    std::vector<std::string_view> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }

    return parse(args);
}

bool Parser::parse(const std::vector<std::string>& args)
{
    for (auto arg = args.begin(); arg != args.end(); ++arg) {
        if (auto flag = _flags.find(*arg); flag != _flags.end()) {
            if (auto ptr = flag->second.lock()) {
                ptr->raise();
            }
            continue;
        }

        if (auto option = _options.find(*arg); option != _options.end()) {
            ++arg;
            if (arg == args.end()) {
                _err << "no value for " << option->first << "\n";
                break;
            }

            if (auto ptr = option->second.lock()) {
                ptr->set(*arg);
            }
            continue;
        }

        if (isValidFlagMerge(*arg)) {
            auto flags = arg->substr(_flagPrefix.size());
            for (size_t i = 0; i + 1 < flags.size(); i++) {
                if (auto ptr = _flags.at(flags[i]).lock(); ptr) {
                    ptr->raise();
                }
            }

            if (auto f = _flags.find(_flagPrefix + flags.back());
                    f != _flags.end()) {
                if (auto ptr = f->second.lock()) {
                    ptr->raise();
                }
            } else if (auto f = _options.find(_flagPrefix + flags.back());
                    f != _options.end()) {
                ++arg;
                if (arg == args.end()) {
                    _err << "no value for " << f->first << "\n";
                    break;
                }

                if (auto ptr = f->second.lock()) {
                    ptr->set(*arg);
                }
            }
            continue;
        }

        if (_argumentsRead < _arguments.size()) {
            if (auto ptr = _arguments.at(_argumentsRead).lock()) {
                ptr->set(*arg);
            }
        } else {
            _leftovers.push_back(*arg);
        }
        _argumentsRead++;

        // TODO: check required:
        // * all required options set
        // * all required argument read (not less than required number of
        //   arguments is read)
    }
}

bool Parser::isValidFlagMerge(const std::string& flagMerge)
{
    if (flagMerge.substr(_flagPrefix.size()) != _flagPrefix) {
        return false;
    }

    auto flags = flagMerge.substr(_flagPrefix.size());
    if (flags.empty()) {
        return false;
    }

    for (size_t i = 0; i + 1 < flags.size(); i++) {
        if (!_flags.count(_flagPrefix + flags[i])) {
            return false;
        }
    }

    if (!_flags.count(_flagPrefix + flags.back()) &&
            !_options.count(_flagPrefix + flags.back())) {
        return false;
    }

    return true;
}

} // namespace car
