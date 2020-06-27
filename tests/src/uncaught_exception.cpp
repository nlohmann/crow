#include <crow/crow.hpp>

void bar(int i)
{
    if (i == 9)
    {
        throw std::runtime_error("uncaught");
    }
}

void foo()
{
    for (int i = 0; i < 10; ++i)
    {
        bar(i);
    }
}

int main()
{
    crow::init("https://a2bfbfbd7eb74a66b799535fad454555:0d03645ed1d641ee9a74b18fcd9a83c6@sentry.io/1535511");
    crow::install_handlers();

    crow::configure_scope([](crow::scope& scope) {
        scope.set_extra("test_case", "uncaught_exception");
    });

    crow::set_before_send_hook([](crow::types::event_t&) {
        return true;
    });

    foo();
}
