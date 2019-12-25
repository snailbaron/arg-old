#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <arg.hpp>

#include <vector>
#include <string>
#include <utility>

class Args {
public:
    template <class... Ts>
    Args(Ts&&... args)
        : _args{std::forward<Ts>(args)...}
    {
        for (auto& arg : _args) {
            _argv.push_back(arg.data());
        }
    }

    int argc() const
    {
        return _args.size();
    }

    char** argv()
    {
        return _argv.data();
    }

private:
    std::vector<std::string> _args;
    std::vector<char*> _argv;
};

TEST_CASE("Unexpected argument")
{
    auto args = Args{"./program", "-v"};
    arg::parse(args.argc(), args.argv());
    // TODO: must report error
}
