#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <crow/crow.hpp>
#include <doctest/doctest.h>

TEST_CASE("Context")
{
    crow::init("https://a2bfbfbd7eb74a66b799535fad454555:0d03645ed1d641ee9a74b18fcd9a83c6@sentry.io/1535511");

    crow::configure_scope([](crow::scope& scope) {
        scope.set_extra("test_case", "integration_tests");
    });

    crow::types::event_t e;
    e.environment = "development";
    e.message = "this is a test";
    e.context.app.app_identifier = "me.nlohmann.crow.2";
    e.context.app.app_version = "0.0.1";
    e.context.app.app_name = "Crow";
    e.level = crow::types::level_t::error;
    crow::capture_event(e);
}

TEST_CASE("exception")
{
    std::runtime_error ex("division by zero");
    crow::capture_exception(ex);
}
