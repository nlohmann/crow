/*
 _____ _____ _____ _ _ _
|     | __  |     | | | |  Crow - a Sentry client for C++
|   --|    -|  |  | | | |  version 0.0.4
|_____|__|__|_____|_____|  https://github.com/nlohmann/crow

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2018 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <exception>
#include <future>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <crow/config.h>
#include <crow/crow.hpp>
#include "crow_utilities.hpp"
#include <thirdparty/curl_wrapper/curl_wrapper.hpp>
#include <thirdparty/json/json.hpp>

using json = nlohmann::json;

namespace nlohmann
{
crow* crow::m_client_that_installed_termination_handler = nullptr;

crow::crow(const std::string& dsn,
           const json& context,
           const bool install_handlers)
    : m_enabled(not dsn.empty())
{
    // process DSN
    if (not dsn.empty())
    {
        const std::regex dsn_regex("(http[s]?)://([^:]+):([^@]+)@([^/]+)/([0-9]+)");
        std::smatch pieces_match;

        if (std::regex_match(dsn, pieces_match, dsn_regex) and pieces_match.size() == 6)
        {
            const auto scheme = pieces_match.str(1);
            m_public_key = pieces_match.str(2);
            m_secret_key = pieces_match.str(3);
            const auto host = pieces_match.str(4);
            const auto project_id = pieces_match.str(5);
            m_store_url = scheme + "://" + host + "/api/" + project_id + "/store/";
        }
        else
        {
            throw std::invalid_argument("DNS " + dsn + " is invalid");
        }
    }

    // manage context
    clear_context();
    merge_context(context);

    // install termination handler
    if (install_handlers)
    {
        install_handler();
    }
}

void crow::install_handler()
{
    if (existing_termination_handler == nullptr)
    {
        existing_termination_handler = std::set_terminate([]() {});
        std::set_terminate(&new_termination_handler);

        // we remember this client, because we will use it to report
        // uncaught exceptions with it
        m_client_that_installed_termination_handler = this;
    }
}

crow::crow(const crow& other)
    : m_enabled(other.m_enabled),
      m_public_key(other.m_public_key),
      m_secret_key(other.m_secret_key),
      m_store_url(other.m_store_url),
      m_payload(other.m_payload)
{}

crow::~crow()
{
    if (m_pending_future.valid())
    {
        m_pending_future.wait();
    }
}

void crow::capture_message(const std::string& message,
                           const json& attributes,
                           const bool asynchronous)
{
    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload["message"] = message;
    m_payload["event_id"] = nlohmann::crow_utilities::generate_uuid();
    m_payload["timestamp"] = nlohmann::crow_utilities::get_iso8601();

    if (attributes.is_object())
    {
        // logger
        auto logger = attributes.find("logger");
        if (logger != attributes.end())
        {
            m_payload["logger"] = *logger;
        }

        // level
        m_payload["level"] = attributes.value("level", "error");

        // context
        auto context = attributes.find("context");
        if (context != attributes.end())
        {
            merge_context(*context);
        }

        // extra
        auto extra = attributes.find("extra");
        if (extra != attributes.end())
        {
            m_payload["extra"] = *extra;
        }
    }

    m_pending_future = std::async(std::launch::async, [this] { return post(m_payload); });
    if (not asynchronous)
    {
        m_pending_future.wait();
    }
}


void crow::capture_exception(const std::exception& exception,
                             const json& context,
                             const bool asynchronous,
                             const bool handled)
{
    std::stringstream thread_id;
    thread_id << std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload["exception"].push_back({{"type", crow_utilities::pretty_name(typeid(exception).name())},
        {"value", exception.what()},
        {"module", crow_utilities::pretty_name(typeid(exception).name(), true)},
        {"mechanism", {{"handled", handled}, {"description", handled ? "handled exception" : "unhandled exception"}}},
        {"stacktrace", {{"frames", crow_utilities::get_backtrace()}}},
        {"thread_id", thread_id.str()}});
    m_payload["event_id"] = crow_utilities::generate_uuid();
    m_payload["timestamp"] = nlohmann::crow_utilities::get_iso8601();

    // add given context
    merge_context(context);

    m_pending_future = std::async(std::launch::async, [this] { return post(m_payload); });
    if (not asynchronous)
    {
        m_pending_future.wait();
    }
}

void crow::add_breadcrumb(const std::string& message,
                          const json& attributes)
{
    json breadcrumb =
    {
        {"event_id", crow_utilities::generate_uuid()},
        {"message", message},
        {"level", "info"},
        {"type", "default"},
        {"category", "log"},
        {"timestamp", crow_utilities::get_timestamp()}
    };

    if (attributes.is_object())
    {
        // type
        auto type = attributes.find("type");
        if (type != attributes.end())
        {
            breadcrumb["type"] = *type;
        }

        // level
        auto level = attributes.find("level");
        if (level != attributes.end())
        {
            breadcrumb["level"] = *level;
        }

        // category
        auto category = attributes.find("category");
        if (category != attributes.end())
        {
            breadcrumb["category"] = *category;
        }

        // data
        auto data = attributes.find("data");
        if (data != attributes.end())
        {
            breadcrumb["data"] = *data;
        }
    }

    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload["breadcrumbs"]["values"].push_back(std::move(breadcrumb));
}

std::string crow::get_last_event_id() const
{
    if (m_pending_future.valid())
    {
        try
        {
            return json::parse(m_pending_future.get()).at("id");
        }
        catch (const json::exception&)
        {
            return "";
        }
    }
    else
    {
        return "";
    }
}

const json& crow::get_context() const
{
    return m_payload;
}

void crow::add_user_context(const json& data)
{
    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload["user"].update(data);
}

void crow::add_tags_context(const json& data)
{
    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload["tags"].update(data);
}

void crow::add_request_context(const json& data)
{
    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload["request"].update(data);
}

void crow::add_extra_context(const json& data)
{
    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload["extra"].update(data);
}

void crow::merge_context(const json& context)
{
    if (context.is_object())
    {
        std::lock_guard<std::mutex> lock(m_payload_mutex);
        for (const auto& el : context.items())
        {
            if (el.key() == "user" or el.key() == "request" or el.key() == "extra" or el.key() == "tags")
            {
                m_payload[el.key()].update(el.value());
            }
            else
            {
                throw std::runtime_error("invalid context");
            }
        }
    }
}

void crow::clear_context()
{
    std::lock_guard<std::mutex> lock(m_payload_mutex);
    m_payload.clear();
    m_payload["platform"] = "c";
    m_payload["sdk"]["name"] = "crow";
    m_payload["sdk"]["version"] = NLOHMANN_CROW_VERSION;

    // add context: app
    m_payload["contexts"]["app"]["build_type"] = NLOHMANN_CROW_CMAKE_BUILD_TYPE;
    m_payload["contexts"]["app"]["pointer_size"] = NLOHMANN_CROW_BITS;

    // add context: device
    m_payload["contexts"]["device"]["arch"] = NLOHMANN_CROW_CMAKE_SYSTEM_PROCESSOR;
    m_payload["contexts"]["device"]["name"] = NLOHMANN_CROW_HOSTNAME;
    m_payload["contexts"]["device"]["model"] = NLOHMANN_CROW_SYSCTL_HW_MODEL;
    m_payload["contexts"]["device"]["memory_size"] = NLOHMANN_CROW_TOTAL_PHYSICAL_MEMORY;

    // add context: os
    m_payload["contexts"]["os"]["name"] = NLOHMANN_CROW_CMAKE_SYSTEM_NAME;
    m_payload["contexts"]["os"]["version"] = NLOHMANN_CROW_OS_RELEASE;
    if (not std::string(NLOHMANN_CROW_OS_VERSION).empty())
    {
        m_payload["contexts"]["os"]["build"] = NLOHMANN_CROW_OS_VERSION;
    }
    else
    {
        m_payload["contexts"]["os"]["build"] = NLOHMANN_CROW_CMAKE_SYSTEM_VERSION;
    }
    if (not std::string(NLOHMANN_CROW_UNAME).empty())
    {
        m_payload["contexts"]["os"]["kernel_version"] = NLOHMANN_CROW_UNAME;
    }
    else if (not std::string(NLOHMANN_CROW_SYSTEMINFO).empty())
    {
        m_payload["contexts"]["os"]["kernel_version"] = NLOHMANN_CROW_SYSTEMINFO;
    }

    // add context: runtime
    m_payload["contexts"]["runtime"]["name"] = NLOHMANN_CROW_CMAKE_CXX_COMPILER_ID;
    m_payload["contexts"]["runtime"]["version"] = NLOHMANN_CROW_CMAKE_CXX_COMPILER_VERSION;
    m_payload["contexts"]["runtime"]["detail"] = NLOHMANN_CROW_CXX;

    // add context: user
    const char* user = getenv("USER");
    if (user == nullptr)
    {
        user = getenv("USERNAME");
    }
    if (user)
    {
        m_payload["user"]["id"] = std::string(user) + "@" + NLOHMANN_CROW_HOSTNAME;
        m_payload["user"]["username"] = user;
    }
}

std::string crow::post(const json& payload)
{
    if (m_enabled)
    {
        curl_wrapper curl;

        // add security header
        std::string security_header = "X-Sentry-Auth: Sentry sentry_version=5,sentry_client=crow/";
        security_header += std::string(NLOHMANN_CROW_VERSION) + ",sentry_timestamp=";
        security_header += std::to_string(crow_utilities::get_timestamp());
        security_header += ",sentry_key=" + m_public_key;
        security_header += ",sentry_secret=" + m_secret_key;
        curl.set_header(security_header.c_str());

        return curl.post(m_store_url, payload);
    }

    return "";
}

void crow::new_termination_handler()
{
    assert(m_client_that_installed_termination_handler != nullptr);

    auto current_ex = std::current_exception();
    if (current_ex)
    {
        m_client_that_installed_termination_handler->add_breadcrumb("uncaught exception", {{"type", "exceptiomn"}, {"level", "critical"}});
        try
        {
            std::rethrow_exception(current_ex);
        }
        catch (const std::exception& e)
        {
            m_client_that_installed_termination_handler->capture_exception(e, nullptr, false, false);
        }
    }

    m_client_that_installed_termination_handler->existing_termination_handler();
}

}