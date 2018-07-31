#include <crow/crow.hpp>

int main()
{
    auto c = crow("https://44ad28adb2b64c91a725502445eec371:13bc69cc502d4fb694eb682f8916bce1@sentry.io/1243737");
    std::cout << "created client" << std::endl;

    c.add_breadcrumb(__DATE__ " " __TIME__);
    std::cout << "added breadcrumb" << std::endl;

    c.capture_message("This is a live test.");
    std::cout << "sent message" << std::endl;

    std::cout << "last event id: " << c.get_last_event_id() << std::endl;
}
