#include <crow/crow.hpp>

int main()
{
    auto c = crow("https://fad7ed01056940969a519aba36dc0b2f:3787b21e465845a09d781ab9eb048ae7@sentry.io/1253079");
    std::cout << "created client" << std::endl;

    c.add_breadcrumb(__DATE__ " " __TIME__);
    std::cout << "added breadcrumb" << std::endl;

    c.capture_message(NLOHMANN_CROW_CXX);
    std::cout << "sent message" << std::endl;

    std::cout << "last event id: " << c.get_last_event_id() << std::endl;
}
