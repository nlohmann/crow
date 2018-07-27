#include <crow/crow.hpp>


int main()
{
    crow s("https://44ad28adb2b64c91a725502445eec371:13bc69cc502d4fb694eb682f8916bce1@sentry.io/1243737");

    s.add_breadcrumb("entering main");

    try {
        json::parse("parse error");
    } catch(const std::exception& e)
    {
        s.capture_exception(e);
    }

    s.add_breadcrumb("leaving main");

    throw std::runtime_error("boom");

    return 0;

    s.add_breadcrumb("entering main", "navigation", {{"from", "void"}, {"to", "main"}});
    s.add_breadcrumb("leaving main");
    s.capture_message("hello, world 6!");
}