#include <arg.hpp>

int main(int argc, char* argv[])
{
    arg::help("-h", "--help");
    auto flag = arg::flag("-f", "--flag")
        .help("flag");
    auto multiFlag = arg::multiFlag("-v", "--verbose")
        .help("flag that can be used multiple times");
    auto stringOption = arg::option("-o", "--option")
        .help("string option");
    auto integerOption = arg::option<int>("-i", "--integer")
        .help("integer option");
    auto requiredParameter = arg::option<float>("-p", "--point")
        .required()
        .help("required point");
    auto p1 = arg::argument<int>()
        .required()
        .help("positional argument");
    auto p2 = arg::multiArgument<int>()
        .help("the rest of positional arguments");
    arg::parse(argc, argv);
}
