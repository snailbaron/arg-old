#include <car.hpp>

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
    auto parser = car::Parser{};

    auto optionalFlag = parser.flag("-f", "--optional-flag")
        .help("optional flag");
    auto requiredFlag = parser.flag("-r", "--required-flag")
        .required()
        .help("required flag");
    auto stringOption = parser.option("-o", "--option")
        .help("string option");
    auto integerOption = parser.option<int>("-i", "--integer")
        .help("integer option");
    auto requiredParameter = parser.option<Point>("-p", "--point")
        .required()
        .help("required point");

    parser.parse(argc, argv);
}
