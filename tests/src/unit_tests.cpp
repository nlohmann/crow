#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <crow/client/client.hpp>
#include <crow/types/event.hpp>
#include <crow/types/interfaces/breadcrumbs.hpp>
#include <crow/types/interfaces/gpu.hpp>
#include <crow/types/interfaces/http.hpp>
#include <crow/types/interfaces/sdk.hpp>
#include <crow/types/interfaces/template.hpp>
#include <crow/types/interfaces/user.hpp>
#include <doctest/doctest.h>

using namespace crow::types;

TEST_CASE("Breadcrumbs Interface")
{
    interfaces::breadcrumb_value_t b1, b2;

    b1.timestamp_unix = 1461185753845;
    b1.message = "Something happened";
    b1.category = "log";
    b1.data["foo"] = "bar";
    b1.data["blub"] = "blah";

    b2.timestamp = "2016-04-20T20:55:53.847Z";
    b2.type = interfaces::breadcrumb_type_t::navigation;
    b2.navigation_data.from = "/login";
    b2.navigation_data.to = "/dashboard";

    interfaces::breadcrumbs_t breadcrumbs = {{b1, b2}};

    CHECK(nlohmann::json(breadcrumbs) == R"(
    {
      "values": [
        {
          "timestamp": 1461185753845,
          "message": "Something happened",
          "category": "log",
          "data": {
            "foo": "bar",
            "blub": "blah"
          },
          "level": "info",
          "type": "default"
        },
        {
          "timestamp": "2016-04-20T20:55:53.847Z",
          "level": "info",
          "type": "navigation",
          "data": {
            "from": "/login",
            "to": "/dashboard"
          }
        }
      ]
    }
    )"_json);
}

TEST_CASE("GPU interface")
{
    interfaces::gpu_t gpu;
    gpu.name = "AMD Radeon Pro 560";
    gpu.vendor_name = "Apple";
    gpu.memory_size = 4096;
    gpu.api_type = "Metal";
    gpu.multi_threaded_rendering = true;
    gpu.version = "Metal";
    gpu.npot_support = "Full";

    CHECK(nlohmann::json(gpu) == R"(
    {
        "api_type": "Metal",
        "memory_size": 4096,
        "multi_threaded_rendering": true,
        "name": "AMD Radeon Pro 560",
        "npot_support": "Full",
        "vendor_name": "Apple",
        "version": "Metal"
    }
    )"_json);
}

TEST_CASE("HTTP Interface")
{
    interfaces::http_t http;
    http.url = "http://absolute.uri/foo";
    http.method = "POST";
    http.data["foo"] = "bar";
    http.query_string = "hello=world";
    http.cookies = "foo=bar";
    http.headers["Content-Type"] = "text/html";
    http.env["REMOTE_ADDR"] = "192.168.0.1";

    CHECK(nlohmann::json(http) == R"(
    {
        "cookies": "foo=bar",
        "data": {
            "foo": "bar"
        },
        "env": {
            "REMOTE_ADDR": "192.168.0.1"
        },
        "headers": {
            "Content-Type": "text/html"
        },
        "method": "POST",
        "query_string": "hello=world",
        "url": "http://absolute.uri/foo"
    }
    )"_json);
}

TEST_CASE("SDK Interface")
{
    interfaces::sdk_t sdk;

    sdk.name = "sentry.javscript.react-native";
    sdk.version = "1.0.0";
    sdk.integrations = {"redux"};
    sdk.packages = {{"npm:@sentry/react-native", "0.39.0"},
                    {"git:https://github.com/getsentry/sentry-cocoa.git", "4.1.0"}};

    CHECK(nlohmann::json(sdk) == R"(
    {
        "integrations": [
            "redux"
        ],
        "name": "sentry.javscript.react-native",
        "packages": [
            {
                "name": "npm:@sentry/react-native",
                "version": "0.39.0"
            },
            {
                "name": "git:https://github.com/getsentry/sentry-cocoa.git",
                "version": "4.1.0"
            }
        ],
        "version": "1.0.0"
    }
    )"_json);
}

TEST_CASE("Template Interface")
{
    interfaces::template_t t;

    t.abs_path = "/real/file/name.html";
    t.filename = "file/name.html";
    t.pre_context = {"line1", "line2"};
    t.context_line = "line3";
    t.lineno = 3;
    t.post_context = {"line4", "line5"};

    CHECK(nlohmann::json(t) == R"(
    {
      "abs_path": "/real/file/name.html",
      "filename": "file/name.html",
      "pre_context": [
        "line1",
        "line2"
      ],
      "context_line": "line3",
      "lineno": 3,
      "post_context": [
        "line4",
        "line5"
      ]
    }
    )"_json);
}

TEST_CASE("User Interface")
{
    interfaces::user_t user;

    user.id = "unique_id";
    user.username = "my_user";
    user.email = "foo@example.com";
    user.ip_address = "127.0.0.1";
    user.extra["subscription"] = "basic";

    CHECK(nlohmann::json(user) == R"(
    {
        "email": "foo@example.com",
        "id": "unique_id",
        "ip_address": "127.0.0.1",
        "subscription": "basic",
        "username": "my_user"
    }
    )"_json);
}

TEST_CASE("DSN")
{
    CHECK_NOTHROW(crow::client("https://a2bfbfbd7eb74a66b799535fad454555:0d03645ed1d641ee9a74b18fcd9a83c6@sentry.io/1535511"));
    CHECK_NOTHROW(crow::client("https://a2bfbfbd7eb74a66b799535fad454555@sentry.io/1535511"));
    CHECK_THROWS_AS(crow::client("https://sentry.io/1535511"), std::invalid_argument);
}