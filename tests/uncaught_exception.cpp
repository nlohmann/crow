#include <cassert>
#include <exception>
#include <crow/crow.hpp>

using crow = nlohmann::crow;

crow* client = nullptr;

// a termination handler that checks the state of the messages sent to Sentry
void my_termination_handler()
{
    std::string last_id = client->get_last_event_id();
    const bool success = not last_id.empty();
    delete client;
    std::exit(success ? 0 : 1);
}

int main()
{
    // register our checking termination handler
    std::set_terminate(my_termination_handler);

    // define a client and install the handlers
    client = new crow("http://abc:def@127.0.0.1:5000/1");
    client->install_handler();

    // throw an uncaught exception
    throw std::runtime_error("oops!");
}
