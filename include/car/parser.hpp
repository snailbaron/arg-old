#pragma once

#include <car/flag.hpp>
#include <car/value.hpp>
#include <car/multivalue.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace car {

class Parser {
public:
    bool parse(int argc, char* argv[]);

    template <class Args>
    bool parse(const Args& args)
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
                    _err << "no value for " << *arg << "\n";
                    break;
                }

                if (auto ptr = option->second.lock()) {
                    ptr->set(*arg);
                }
                continue;
            }

            if (auto subargs = splitMergedArgs(*arg); !subargs.empty()) {
                // TODO: process all except the last one as flags; the last one as
                // a flag or an option.
            }

            if (_argumentsRead < _arguments.size()) {
                if (auto ptr = _arguments.at(_argumentsRead).lock()) {
                    ptr->set(*arg);
                }
            } else {
                _leftovers.push_back(*arg);
            }
            _argumentsRead++;
        }
    }

    void printUsage() const;

private:
    std::ostream& _err = std::cerr;

    std::string _programName;
    std::map<std::string, std::weak_ptr<Flag>> _flags;
    std::map<std::string, std::weak_ptr<AbstractValue>> _options;
    std::vector<std::weak_ptr<AbstractValue>> _arguments;
    size_t _argumentsRead = 0;
    std::vector<std::string> _leftovers;
};

} // namespace car
