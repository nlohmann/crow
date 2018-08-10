#include <exception>
#include <crow/crow.hpp>
#include <iostream>
#include <thirdparty/curl_wrapper/curl_wrapper.hpp>

using crow = nlohmann::crow;
crow *c = nullptr;

// a termination handler that checks the state of the messages sent to Sentry
void my_termination_handler()
{
    delete c;
    std::lock_guard<std::mutex> lock(curl_wrapper::mutex);
    std::cout << "payload = " << std::setw(4) << curl_wrapper::results << std::endl;
    if (curl_wrapper::results[0]["payload"]["exception"][0]["value"] != "oops!")
    {
        std::exit(1);
    }
    else
    {
        std::exit(0);
    }
}

int main()
{
    // register our checking termination handler
    std::set_terminate(my_termination_handler);

    // define a client and install the handlers
    c = new crow("http://abc:def@sentry.io/123", nullptr, 1.0, true);

    // throw an uncaught exception
    throw std::runtime_error("oops!");
}
