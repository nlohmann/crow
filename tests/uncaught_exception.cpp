#include <exception>
#include <crow/crow.hpp>
#include <iostream>

using crow = nlohmann::crow;
json results = json::array();

// a termination handler that checks the state of the messages sent to Sentry
void my_termination_handler()
{
    std::cout << "payload = " << std::setw(4) << results << std::endl;
    assert(results[0]["payload"]["exception"][0]["value"] == "oops!");
    std::exit(0);
}

int main()
{
    // register our checking termination handler
    std::set_terminate(my_termination_handler);

    // define a client and install the handlers
    crow c("http://abc:def@sentry.io/123", nullptr, 1.0, true);

    // throw an uncaught exception
    throw std::runtime_error("oops!");
}
