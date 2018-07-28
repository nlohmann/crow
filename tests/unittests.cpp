#define CATCH_CONFIG_MAIN

#include <thirdparty/catch/catch.hpp>
#include <crow/crow.hpp>
#include <thirdparty/json/json.hpp>

using json = nlohmann::json;
json results;

void verify_message_structure(const json& msg);
void verify_message_structure(const json& msg)
{
    CAPTURE(msg);
    CHECK(msg.at("event_id").is_string());
    CHECK(msg.at("timestamp").is_string());
    //CHECK(msg.at("logger").is_string());
    CHECK(msg.at("platform").is_string());

    CHECK(msg.at("sdk").at("name").is_string());
    CHECK(msg.at("sdk").at("version").is_string());
}

TEST_CASE("DSN parsing")
{
    SECTION("valid DSN")
    {
        CHECK_NOTHROW(crow("http://abc:def@sentry.io/123"));
        CHECK_NOTHROW(crow("https://abc:def@sentry.io/123"));
    }

    SECTION("invalid DSN")
    {
        // missing protocol
        CHECK_THROWS_AS(crow("://abc:def@sentry.io/123"), std::invalid_argument);

        // unsupported protocol
        CHECK_THROWS_AS(crow("gopher://abc:def@sentry.io/123"), std::invalid_argument);

        // missing public key
        CHECK_THROWS_AS(crow("https://:def@sentry.io/123"), std::invalid_argument);

        // missing private key
        CHECK_THROWS_AS(crow("https://abc:@sentry.io/123"), std::invalid_argument);

        // missing public and private key
        CHECK_THROWS_AS(crow("https://sentry.io/123"), std::invalid_argument);

        // missing project
        CHECK_THROWS_AS(crow("https://abc:def@sentry.io"), std::invalid_argument);
        CHECK_THROWS_AS(crow("https://abc:def@sentry.io/"), std::invalid_argument);
    }
}

TEST_CASE("creating messages")
{
    crow crow_client("https://abc:def@sentry.io/123");
    std::string url = "https://sentry.io/api/123/store/";

    SECTION("capture_message")
    {
        SECTION("without payload")
        {
            results.clear();
            std::string msg_string = "message text";
            crow_client.capture_message(msg_string, nullptr, false);

            CHECK(results.size() == 1);
            const auto& message = results.at(0).at("payload");
            verify_message_structure(message);
            CHECK(message.at("message") == msg_string);
            CHECK(results.at(0).at("url") == url);
        }

        SECTION("with payload")
        {
            results.clear();
            std::string msg_string = "message text";
            json payload = {{"key", "value"}};
            crow_client.capture_message(msg_string, payload, false);

            CHECK(results.size() == 1);
            const auto& message = results.at(0).at("payload");
            verify_message_structure(message);
            CHECK(message.at("message") == msg_string);
            for (auto& el : payload.items())
            {
                CHECK(message.at(el.key()) == el.value());
            }
            CHECK(results.at(0).at("url") == url);
        }
    }

    SECTION("capture_exception")
    {
        SECTION("marked as handled")
        {
            results.clear();
            std::string ex_string = "exception text";
            crow_client.capture_exception(std::runtime_error(ex_string), false, true);

            CHECK(results.size() == 1);
            const auto& message = results.at(0).at("payload");
            CAPTURE(message);
            verify_message_structure(message);
            CHECK(message.at("exception").size() == 1);
            const auto& exception = message.at("exception").at(0);
#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
            CHECK(exception.at("type") == "std::runtime_error");
#endif
            CHECK(exception.at("value") == ex_string);
            CHECK(exception.at("mechanism").at("handled"));
            CHECK(results.at(0).at("url") == url);
        }

        SECTION("marked as unhandled")
        {
            results.clear();
            std::string ex_string = "exception text";
            crow_client.capture_exception(std::runtime_error(ex_string), false, false);

            const auto& message = results.at(0).at("payload");
            CAPTURE(message);
            verify_message_structure(message);
            CHECK(message.at("exception").size() == 1);
            const auto& exception = message.at("exception").at(0);
#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
            CHECK(exception.at("type") == "std::runtime_error");
#endif
            CHECK(exception.at("value") == ex_string);
            CHECK(not exception.at("mechanism").at("handled"));
            CHECK(results.at(0).at("url") == url);
        }
    }

    SECTION("add_breadcrumb")
    {
        results.clear();
        const std::string msg1 = "breadcrumb 1";
        const std::string msg2 = "breadcrumb 2";
        const json data2 = {{"from", "origin"}, {"to", "destination"}};

        // add breadcrumbs
        crow_client.add_breadcrumb(msg1);
        crow_client.add_breadcrumb(msg2, "navigation", data2);

        // capture message
        std::string msg_string = "message text";
        crow_client.capture_message(msg_string, nullptr, false);

        CHECK(results.size() == 1);
        const auto& message = results.at(0).at("payload");
        verify_message_structure(message);
        CHECK(message.at("message") == msg_string);
        CHECK(results.at(0).at("url") == url);

        CHECK(message.at("breadcrumbs").at("values").size() == 2);
        CHECK(message.at("breadcrumbs").at("values").at(0).at("message") == msg1);
        CHECK(message.at("breadcrumbs").at("values").at(0).at("type") == "default");

        CHECK(message.at("breadcrumbs").at("values").at(1).at("message") == msg2);
        CHECK(message.at("breadcrumbs").at("values").at(1).at("type") == "navigation");
        CHECK(message.at("breadcrumbs").at("values").at(1).at("data") == data2);
    }
}
