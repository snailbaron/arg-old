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



} // namespace car
