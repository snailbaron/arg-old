#include <arg.hpp>

#include <istream>

struct Point {
    int x;
    int y;
};

std::istream& operator>>(std::istream& input, Point& point)
{
    return input >> point.x >> point.y;
}

int main(int argc, char* argv[])
{
    auto parser = arg::Parser{};

    auto flag = parser.flag("-f", "--flag")
        .help("flag");
    auto stringOption = parser.option("-o", "--option")
        .help("string option");
    auto integerOption = parser.option<int>("-i", "--integer")
        .help("integer option");
    auto requiredParameter = parser.option<Point>("-p", "--point")
        .required()
        .help("required point");

    parser.parse(argc, argv);
}
