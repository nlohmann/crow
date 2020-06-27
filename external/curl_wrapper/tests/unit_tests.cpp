#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <algorithm>
#include <functional>
#include <random>
#include <doctest/doctest.h>
#include <http/http.hpp>

using json = nlohmann::json;

TEST_CASE("constructor")
{
    CHECK_NOTHROW(http::request());
}

TEST_CASE("reset")
{
    http::request request;
    CHECK_NOTHROW(request.reset());
}

TEST_CASE("options")
{
    http::request request;
    CHECK_NOTHROW(request.be_verbose(false));
}

TEST_CASE("status codes")
{
    http::request request;

    SUBCASE("GET")
    {
        SUBCASE("200/OK")
        {
            const auto url = "https://httpbin.org/status/200";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 200);
            CHECK(response.status_code == http::status::OK);
            CHECK(response.reason == "OK");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("201/CREATED")
        {
            const auto url = "https://httpbin.org/status/201";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 201);
            CHECK(response.status_code == http::status::CREATED);
            CHECK(response.reason == "Created");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("202/ACCEPTED")
        {
            const auto url = "https://httpbin.org/status/202";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 202);
            CHECK(response.status_code == http::status::ACCEPTED);
            CHECK(response.reason == "Accepted");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("203/NON AUTHORITATIVE INFORMATION")
        {
            const auto url = "https://httpbin.org/status/203";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 203);
            CHECK(response.status_code == http::status::NON_AUTHORITATIVE_INFORMATION);
            CHECK(response.reason == "Non-Authoritative Information");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("204/NO CONTENT")
        {
            const auto url = "https://httpbin.org/status/204";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 204);
            CHECK(response.status_code == http::status::NO_CONTENT);
            CHECK(response.reason == "No Content");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("205/RESET CONTENT")
        {
            const auto url = "https://httpbin.org/status/205";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 205);
            CHECK(response.status_code == http::status::RESET_CONTENT);
            CHECK(response.reason == "Reset Content");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("206/PARTIAL CONTENT")
        {
            const auto url = "https://httpbin.org/status/206";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 206);
            CHECK(response.status_code == http::status::PARTIAL_CONTENT);
            CHECK(response.reason == "Partial Content");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("300/MULTIPLE CHOICES")
        {
            request.follow_redirects(false);
            const auto url = "https://httpbin.org/status/300";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 300);
            CHECK(response.status_code == http::status::MULTIPLE_CHOICES);
            CHECK(response.reason == "Multiple Choices");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("301/MOVED PERMANENTLY")
        {
            request.follow_redirects(false);
            const auto url = "https://httpbin.org/status/301";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 301);
            CHECK(response.status_code == http::status::MOVED_PERMANENTLY);
            CHECK(response.reason == "Moved Permanently");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("302/MOVED TEMPORARILY")
        {
            request.follow_redirects(false);
            const auto url = "https://httpbin.org/status/302";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 302);
            CHECK(response.status_code == http::status::MOVED_TEMPORARILY);
            CHECK(response.reason == "Moved Temporarily");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("303/SEE OTHER")
        {
            request.follow_redirects(false);
            const auto url = "https://httpbin.org/status/303";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 303);
            CHECK(response.status_code == http::status::SEE_OTHER);
            CHECK(response.reason == "See Other");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("304/NOT MODIFIED")
        {
            request.follow_redirects(false);
            const auto url = "https://httpbin.org/status/304";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 304);
            CHECK(response.status_code == http::status::NOT_MODIFIED);
            CHECK(response.reason == "Not Modified");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("305/USE PROXY")
        {
            request.follow_redirects(false);
            const auto url = "https://httpbin.org/status/305";
            const auto response = request.get(url);
            CHECK(response.ok);
            CHECK(response.status_code == 305);
            CHECK(response.status_code == http::status::USE_PROXY);
            CHECK(response.reason == "Use Proxy");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }

        SUBCASE("400/BAD REQUEST")
        {
            const auto url = "https://httpbin.org/status/400";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 400);
            CHECK(response.status_code == http::status::BAD_REQUEST);
            CHECK(response.reason == "Bad Request");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("401/UNAUTHORIZED")
        {
            const auto url = "https://httpbin.org/status/401";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 401);
            CHECK(response.status_code == http::status::UNAUTHORIZED);
            CHECK(response.reason == "Unauthorized");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("402/PAYMENT REQUIRED")
        {
            const auto url = "https://httpbin.org/status/402";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 402);
            CHECK(response.status_code == http::status::PAYMENT_REQUIRED);
            CHECK(response.reason == "Payment Required");
            CHECK(response.text == "Fuck you, pay me!");
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("403/FORBIDDEN")
        {
            const auto url = "https://httpbin.org/status/403";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 403);
            CHECK(response.status_code == http::status::FORBIDDEN);
            CHECK(response.reason == "Forbidden");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("404/NOT FOUND")
        {
            const auto url = "https://httpbin.org/status/404";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 404);
            CHECK(response.status_code == http::status::NOT_FOUND);
            CHECK(response.reason == "Not Found");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("405/METHOD NOT ALLOWED")
        {
            const auto url = "https://httpbin.org/status/405";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 405);
            CHECK(response.status_code == http::status::METHOD_NOT_ALLOWED);
            CHECK(response.reason == "Method Not Allowed");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("406/NOT ACCEPTABLE")
        {
            const auto url = "https://httpbin.org/status/406";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 406);
            CHECK(response.status_code == http::status::NOT_ACCEPTABLE);
            CHECK(response.reason == "Not Acceptable");
            CHECK(not response.text.empty());
            CHECK(response.json().at("message") == "Client did not request a supported media type.");
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("407/PROXY AUTHENTICATION REQUIRED")
        {
            const auto url = "https://httpbin.org/status/407";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 407);
            CHECK(response.status_code == http::status::PROXY_AUTHENTICATION_REQUIRED);
            CHECK(response.reason == "Proxy Authentication Required");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("408/REQUEST TIMEOUT")
        {
            const auto url = "https://httpbin.org/status/408";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 408);
            CHECK(response.status_code == http::status::REQUEST_TIME_OUT);
            CHECK(response.reason == "Request Timeout");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("409/CONFLICT")
        {
            const auto url = "https://httpbin.org/status/409";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 409);
            CHECK(response.status_code == http::status::CONFLICT);
            CHECK(response.reason == "Conflict");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("410/GONE")
        {
            const auto url = "https://httpbin.org/status/410";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 410);
            CHECK(response.status_code == http::status::GONE);
            CHECK(response.reason == "Gone");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("411/LENGTH REQUIRED")
        {
            const auto url = "https://httpbin.org/status/411";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 411);
            CHECK(response.status_code == http::status::LENGTH_REQUIRED);
            CHECK(response.reason == "Length Required");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("412/PRECONDITION FAILED")
        {
            const auto url = "https://httpbin.org/status/412";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 412);
            CHECK(response.status_code == http::status::PRECONDITION_FAILED);
            CHECK(response.reason == "Precondition Failed");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("413/REQUEST ENTITY TOO LARGE")
        {
            const auto url = "https://httpbin.org/status/413";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 413);
            CHECK(response.status_code == http::status::REQUEST_ENTITY_TOO_LARGE);
            CHECK(response.reason == "Request Entity Too Large");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("414/REQUEST URI TOO LARGE")
        {
            const auto url = "https://httpbin.org/status/414";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 414);
            CHECK(response.status_code == http::status::REQUEST_URI_TOO_LARGE);
            CHECK(response.reason == "URI Too Long");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("415/UNSUPPORTED MEDIA TYPE")
        {
            const auto url = "https://httpbin.org/status/415";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 415);
            CHECK(response.status_code == http::status::UNSUPPORTED_MEDIA_TYPE);
            CHECK(response.reason == "Unsupported Media Type");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "client error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("500/INTERNAL SERVER ERROR")
        {
            const auto url = "https://httpbin.org/status/500";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 500);
            CHECK(response.status_code == http::status::INTERNAL_SERVER_ERROR);
            CHECK(response.reason == "Internal Server Error");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "server error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("501/NOT IMPLEMENTED")
        {
            const auto url = "https://httpbin.org/status/501";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 501);
            CHECK(response.status_code == http::status::NOT_IMPLEMENTED);
            CHECK(response.reason == "Not Implemented");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "server error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("502/BAD GATEWAY")
        {
            const auto url = "https://httpbin.org/status/502";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 502);
            CHECK(response.status_code == http::status::BAD_GATEWAY);
            CHECK(response.reason == "Bad Gateway");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "server error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("503/SERVICE UNAVAILABLE")
        {
            const auto url = "https://httpbin.org/status/503";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 503);
            CHECK(response.status_code == http::status::SERVICE_UNAVAILABLE);
            CHECK(response.reason == "Service Unavailable");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "server error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("504/GATEWAY TIME OUT")
        {
            const auto url = "https://httpbin.org/status/504";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 504);
            CHECK(response.status_code == http::status::GATEWAY_TIME_OUT);
            CHECK(response.reason == "Gateway Timeout");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "server error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("505/HTTP VERSION NOT SUPPORTED")
        {
            const auto url = "https://httpbin.org/status/505";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 505);
            CHECK(response.status_code == http::status::HTTP_VERSION_NOT_SUPPORTED);
            CHECK(response.reason == "HTTP Version Not Supported");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_THROWS_AS(response.throw_for_status(), http::status_exception);
            CHECK_THROWS_WITH(response.throw_for_status(), "server error");
            try
            {
                response.throw_for_status();
            }
            catch (http::status_exception& e)
            {
                CHECK(e.status_code == response.status_code);
                CHECK(e.reason == response.reason);
                CHECK(e.response != nullptr);
                CHECK(e.response->status_code == response.status_code);
            }
        }

        SUBCASE("600 (invalid status code)")
        {
            const auto url = "https://httpbin.org/status/600";
            const auto response = request.get(url);
            CHECK(not response.ok);
            CHECK(response.status_code == 600);
            CHECK(response.reason == "unknown");
            CHECK(response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));
            CHECK_NOTHROW(response.throw_for_status());
        }
    }
}

TEST_CASE("timeout")
{
    http::request request;

    SUBCASE("GET")
    {
        request.timeout(std::chrono::milliseconds(1));
        const auto url = "https://httpstat.us/200?sleep=5000";
        CHECK_THROWS_AS(request.get(url), http::timeout_exception);
        CHECK_THROWS_WITH(request.get(url), "Timeout was reached");

        try
        {
            request.get(url);
        }
        catch (http::timeout_exception& e)
        {
            CHECK(not std::string(e.details).empty());
            CHECK(e.curl_code == CURLE_OPERATION_TIMEDOUT);
            CHECK(e.response == nullptr);
        }
    }
}

TEST_CASE("response inspection")
{
    http::request request;

    SUBCASE("GET")
    {
        const auto url = "https://httpbin.org/anything";
        const auto response = request.get(url);

        CHECK(response.ok);
        CHECK(not response.text.empty());
        CHECK(response.url == url);
        CHECK(response.elapsed > std::chrono::seconds(0));

        CHECK_NOTHROW(response.json());
        const auto j = response.json();
        CHECK(j.at("method") == "GET");
        CHECK(j.at("url") == "https://httpbin.org/anything");
        CHECK(j.at("args") == json::object());
        CHECK(j.at("data") == "");
        CHECK(j.at("files") == json::object());
        CHECK(j.at("form") == json::object());
        CHECK(j.at("json") == json());
    }

    SUBCASE("POST")
    {
        SUBCASE("form")
        {
            const std::string payload = "hello=world";
            const auto url = "https://httpbin.org/anything";
            const auto response = request.post(url, payload);

            CHECK(response.ok);
            CHECK(not response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_NOTHROW(response.json());
            const auto j = response.json();
            CHECK(j.at("method") == "POST");
            CHECK(j.at("url") == "https://httpbin.org/anything");
            CHECK(j.at("args") == json::object());
            CHECK(j.at("data") == "");
            CHECK(j.at("files") == json::object());
            CHECK(j.at("form") == R"({"hello":"world"})"_json);
            CHECK(j.at("json") == json());
            CHECK(j.at("headers").at("Content-Type") == "application/x-www-form-urlencoded");
        }

        SUBCASE("form, compressed")
        {
            const std::string payload = "hello=world";
            const auto url = "https://httpbin.org/anything";
            const auto response = request.use_compression(true).post(url, payload);

            CHECK(response.ok);
            CHECK(not response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_NOTHROW(response.json());
            const auto j = response.json();
            CHECK(j.at("method") == "POST");
            CHECK(j.at("url") == "https://httpbin.org/anything");
            CHECK(j.at("args") == json::object());
            CHECK(j.at("data") == "");
            CHECK(j.at("files") == json::object());
            CHECK(j.at("json") == json());
            CHECK(j.at("headers").at("Content-Encoding") == "gzip");
            CHECK(j.at("headers").at("Content-Type") == "application/x-www-form-urlencoded");
        }

        SUBCASE("JSON")
        {
            const json payload = R"({"hello":"world"})"_json;
            const auto url = "https://httpbin.org/anything";
            const auto response = request.post(url, payload);

            CHECK(response.ok);
            CHECK(not response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_NOTHROW(response.json());
            const auto j = response.json();
            CHECK(j.at("method") == "POST");
            CHECK(j.at("url") == "https://httpbin.org/anything");
            CHECK(j.at("args") == json::object());
            CHECK(j.at("data") == "{\"hello\":\"world\"}");
            CHECK(j.at("files") == json::object());
            CHECK(j.at("form") == json::object());
            CHECK(j.at("json") == payload);
            CHECK(j.at("headers").at("Content-Type") == "application/json");
        }

        SUBCASE("JSON, compressed")
        {
            const json payload = R"({"hello":"world"})"_json;
            const auto url = "https://httpbin.org/anything";
            const auto response = request.use_compression(true).post(url, payload);

            CHECK(response.ok);
            CHECK(not response.text.empty());
            CHECK(response.url == url);
            CHECK(response.elapsed > std::chrono::seconds(0));

            CHECK_NOTHROW(response.json());
            const auto j = response.json();
            CHECK(j.at("method") == "POST");
            CHECK(j.at("url") == "https://httpbin.org/anything");
            CHECK(j.at("args") == json::object());
            CHECK(j.at("files") == json::object());
            CHECK(j.at("form") == json::object());
            CHECK(j.at("json") == json());
            CHECK(j.at("headers").at("Content-Encoding") == "gzip");
            CHECK(j.at("headers").at("Content-Type") == "application/json");
        }
    }
}

TEST_CASE("redirects")
{
    http::request request;

    SUBCASE("301/Moved Permanently")
    {
        const auto url = "https://httpbin.org/redirect-to?url=https%3A%2F%2Fhttpbin.org%2Fanything&status_code=301";
        const auto response = request.get(url);

        CHECK(response.ok);
        CHECK(response.url == "https://httpbin.org/anything");
    }

    SUBCASE("302/Moved Temporarily")
    {
        const auto url = "https://httpbin.org/redirect-to?url=https%3A%2F%2Fhttpbin.org%2Fanything&status_code=302";
        const auto response = request.get(url);

        CHECK(response.ok);
        CHECK(response.url == "https://httpbin.org/anything");
    }
}

TEST_CASE("other errors")
{
    http::request request;

    SUBCASE("unsupported protocol")
    {
        const auto url = "invalid://127.0.0.1";
        CHECK_THROWS_AS(request.get(url), http::exception);
        CHECK_THROWS_WITH(request.get(url), "Unsupported protocol");

        try
        {
            request.get(url);
        }
        catch (http::exception& e)
        {
            CHECK(not std::string(e.details).empty());
            CHECK(e.curl_code == CURLE_UNSUPPORTED_PROTOCOL);
        }
    }

    SUBCASE("URL malformed")
    {
        CHECK_THROWS_AS(request.get(""), http::exception);
        CHECK_THROWS_WITH(request.get(""), "URL using bad/illegal format or missing URL");
    }
}

TEST_CASE("improve coverage")
{
    SUBCASE("status names")
    {
        // these HTTP statuses cannot be properly retrieved from httpbin.org
        CHECK(std::string(http::status::name(http::status::CONTINUE)) == "Continue");
        CHECK(std::string(http::status::name(http::status::SWITCHING_PROTOCOLS)) == "Switching Protocols");
    }

    SUBCASE("compression for input longer than buffer size")
    {
        http::request request;

        // create string that is hopefully larger than the buffer size after compression
        std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char> rbe;
        std::vector<unsigned char> data(32768 * 2.5);
        std::generate(data.begin(), data.end(), std::ref(rbe));

        // remove null bytes as we would like to create a string
        for (auto& x : data)
        {
            if (x == 0)
            {
                x = 1;
            }
        }

        const std::string payload(data.begin(), data.end());
        const auto url = "https://httpbin.org/anything";
        const auto response = request.use_compression(true).post(url, payload);

        CHECK(response.ok);
        CHECK(not response.text.empty());
        CHECK(response.url == url);
        CHECK(response.elapsed > std::chrono::seconds(0));

        CHECK_NOTHROW(response.json());
        const auto j = response.json();
        CHECK(j.at("method") == "POST");
        CHECK(j.at("url") == "https://httpbin.org/anything");
        CHECK(j.at("headers").at("Content-Encoding") == "gzip");
        CHECK(j.at("headers").at("Content-Type") == "application/x-www-form-urlencoded");
    }
}
