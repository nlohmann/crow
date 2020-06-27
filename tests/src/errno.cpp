#include <cstdio>
#include <crow/crow.hpp>

int main()
{
    crow::types::configuration c;
    c.environment = "dev";

    crow::init("https://a2bfbfbd7eb74a66b799535fad454555:0d03645ed1d641ee9a74b18fcd9a83c6@sentry.io/1535511", c);

    crow::configure_scope([](crow::scope& scope) {
        scope.set_extra("test_case", "errno");
    });

    std::fopen("", "!");
    crow::capture_errno();
}
