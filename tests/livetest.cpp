#include <crow/crow.hpp>

int main()
{
    auto c = crow("https://44ad28adb2b64c91a725502445eec371:13bc69cc502d4fb694eb682f8916bce1@sentry.io/1243737");

    c.add_breadcrumb(__DATE__ " " __TIME__);
    c.capture_message("This is a live test.");
}
