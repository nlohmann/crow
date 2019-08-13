#define CATCH_CONFIG_MAIN

#include <catch/catch.hpp>
#include <crow/crow.hpp>
#include <src/crow_utilities.hpp>

using json = nlohmann::json;
using crow = nlohmann::crow;

json parse_msg(const std::string& raw);
json parse_msg(const std::string& raw)
{
    CAPTURE(raw);
    auto msg = json::parse(raw);

    CAPTURE(msg);
    CHECK(msg.at("event_id").is_string());
    CHECK(msg.at("timestamp").is_string());
    CHECK(msg.at("platform").is_string());

    CHECK(msg.at("sdk").at("name").is_string());
    CHECK(msg.at("sdk").at("version").is_string());

    return msg;
}

TEST_CASE("utilities")
{
    SECTION("get_timestamp")
    {
        auto x = nlohmann::crow_utilities::get_timestamp();
        CAPTURE(x);
        CHECK(x > 1533027000);
    }

    SECTION("get_iso8601")
    {
        auto x = nlohmann::crow_utilities::get_iso8601();
        CAPTURE(x);
        CHECK(x.size() == 20);
        CHECK(x[4] == '-');
        CHECK(x[7] == '-');
        CHECK(x[10] == 'T');
        CHECK(x[13] == ':');
        CHECK(x[16] == ':');
        CHECK(x[19] == 'Z');
        CHECK(x.substr(0, 2) == "20");
    }

    SECTION("generate_uuid")
    {
        auto x = nlohmann::crow_utilities::generate_uuid();
        CAPTURE(x);
        CHECK(x.size() == 32);
        CHECK(x[12] == '4');

        auto y = nlohmann::crow_utilities::generate_uuid();
        CHECK(x != y);
    }
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

TEST_CASE("sample rate")
{
    SECTION("sample rate 0.0")
    {
        crow crow_client("http://abc:def@127.0.0.1:5000/1", nullptr, 0.0);
        crow_client.capture_message("message");

        // make sure no message was sent
        CHECK(crow_client.get_last_event_id().empty());
    }

    SECTION("sample rate 1.0")
    {
        crow crow_client("http://abc:def@127.0.0.1:5000/1", nullptr, 1.0);
        crow_client.capture_message("message");

        // make sure a message was sent
        CHECK(not crow_client.get_last_event_id().empty());
    }
}

TEST_CASE("creating messages")
{
    crow crow_client("http://abc:def@127.0.0.1:5000/1");

    SECTION("capture_message")
    {
        SECTION("without payload")
        {
            std::string msg_string = "message text";
            crow_client.capture_message(msg_string);

            // check payload sent to Sentry
            auto msg = parse_msg(crow_client.get_last_event_id());
            CHECK(msg["message"] == msg_string);
        }
    }

    SECTION("capture_exception")
    {
        SECTION("marked as handled")
        {
            std::string ex_string = "exception text";
            crow_client.capture_exception(std::runtime_error(ex_string), nullptr, true);

            // check payload sent to Sentry
            auto msg = parse_msg(crow_client.get_last_event_id());
            CHECK(msg["exception"][0]["value"] == ex_string);
            CHECK(msg["exception"][0]["mechanism"]["handled"]);
        }

        SECTION("marked as unhandled")
        {
            std::string ex_string = "exception text";
            crow_client.capture_exception(std::runtime_error(ex_string), nullptr, false);

            // check payload sent to Sentry
            auto msg = parse_msg(crow_client.get_last_event_id());
            CHECK(msg["exception"][0]["value"] == ex_string);
            CHECK(not msg["exception"][0]["mechanism"]["handled"]);
        }
    }

    SECTION("add_breadcrumb")
    {
        const std::string msg1 = "breadcrumb 1";
        const std::string msg2 = "breadcrumb 2";
        const json data2 = {{"from", "origin"}, {"to", "destination"}};

        // add breadcrumbs
        crow_client.add_breadcrumb(msg1);
        crow_client.add_breadcrumb(msg2, {{"type", "navigation"}, {"level", "info"}, {"data", data2}});

        // capture message
        std::string msg_string = "message text";
        crow_client.capture_message(msg_string);

        // check payload sent to Sentry
        auto msg = parse_msg(crow_client.get_last_event_id());
        CHECK(msg["breadcrumbs"]["values"].size() == 2);
        CHECK(msg["breadcrumbs"]["values"][0]["message"] == msg1);
        CHECK(msg["breadcrumbs"]["values"][1]["message"] == msg2);
    }
}

TEST_CASE("job list")
{
    crow crow_client("http://abc:def@127.0.0.1:5000/1");
    crow_client.capture_message("message_1");
    crow_client.capture_message("message_2");
    crow_client.capture_message("message_3");
    crow_client.capture_message("message_4");
    crow_client.capture_message("message_5");
    crow_client.capture_message("message_6");
    crow_client.capture_message("message_7");
    crow_client.capture_message("message_8");
    crow_client.capture_message("message_9");
    crow_client.capture_message("message_10");
    crow_client.capture_message("message_11");
}

TEST_CASE("context")
{
    crow crow_client("http://abc:def@127.0.0.1:5000/1");

    SECTION("user context")
    {
        // set email
        const std::string email = "person@example.com";
        crow_client.add_user_context({{"email", email}});

        // capture message
        crow_client.capture_message("msg");

        // check payload sent to Sentry
        auto msg = parse_msg(crow_client.get_last_event_id());
        CHECK(msg["user"]["email"] == email);
    }

    SECTION("tags context")
    {
        // set tag
        const json tag = {{"tag", "value"}};
        crow_client.add_tags_context(tag);

        // capture message
        crow_client.capture_message("msg");

        // check payload sent to Sentry
        auto msg = parse_msg(crow_client.get_last_event_id());
        CHECK(msg["tags"] == tag);
    }

    SECTION("request context")
    {
        // set request context
        crow_client.add_request_context({{"url", "http://example.com"}, {"method", "GET"}});

        // capture message
        crow_client.capture_message("msg");

        // check payload sent to Sentry
        auto msg = parse_msg(crow_client.get_last_event_id());

        // check payload sent to Sentry
        CHECK(msg["request"]["url"] == "http://example.com");
        CHECK(msg["request"]["method"] == "GET");
    }

    SECTION("extra context")
    {
        // set extra context
        const json extra = {{"foo", "bar"}};
        crow_client.add_extra_context(extra);

        // capture message
        crow_client.capture_message("msg");

        // check payload sent to Sentry
        auto msg = parse_msg(crow_client.get_last_event_id());

        CHECK(msg["extra"] == extra);
    }

    SECTION("reset context")
    {
        auto previous_context = crow_client.get_context();

        crow_client.add_user_context({{"email", "person@example.com"}});
        crow_client.add_tags_context({{"tag", "value"}});
        crow_client.add_request_context({{"url", "http://example.com"}, {"method", "GET"}});
        crow_client.add_extra_context({{"foo", "bar"}});

        CHECK(crow_client.get_context() != previous_context);

        crow_client.clear_context();

        CHECK(crow_client.get_context() == previous_context);
    }
}
