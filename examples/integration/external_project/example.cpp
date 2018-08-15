#include <crow/crow.hpp>

using nlohmann::crow;

int main()
{
    crow client("http://abc:cde@sentry.io/42");

    client.add_breadcrumb("this is a breadcrumb");
    client.capture_message("this is a message");
}
