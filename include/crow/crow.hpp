/*
 _____ _____ _____ _ _ _
|     | __  |     | | | |  Crow - a Sentry client for C++
|   --|    -|  |  | | | |  version 0.0.1
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

#ifndef NLOHMANN_CROW_HPP
#define NLOHMANN_CROW_HPP

#include <cerrno>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <exception>
#include <future>
#include <random>
#include <regex>
#include <stdexcept>
#include <typeinfo>
#include <crow/config.h>
#include <thirdparty/curl_wrapper/curl_wrapper.hpp>
#include <thirdparty/json/json.hpp>

#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
    #include <cxxabi.h> // for abi::__cxa_demangle
#endif

#ifdef NLOHMANN_CROW_HAVE_EXECINFO_H
    #include <execinfo.h> // for backtrace
#endif

#ifdef NLOHMANN_CROW_HAVE_DLFCN_H
    #include <dlfcn.h> // for dladdr
#endif

using json = nlohmann::json;

/// namespace for Niels Lohmann
namespace nlohmann
{
class crow;

namespace detail
{
crow* last = nullptr;

// https://gist.github.com/fmela/591333
// This function produces a stack backtrace with demangled function & method names.
json get_backtrace(int skip = 1)
{
    json result = json::array();

#if defined(NLOHMANN_CROW_HAVE_DLFCN_H) && defined(NLOHMANN_CROW_HAVE_EXECINFO_H)
    void* callstack[128];
    const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
    char buf[1024];
    int nFrames = backtrace(callstack, nMaxFrames);
    char** symbols = backtrace_symbols(callstack, nFrames);

    for (int i = skip; i < nFrames; i++)
    {
        Dl_info info;
        if (dladdr(callstack[i], &info) && info.dli_sname)
        {
            char* demangled = nullptr;
            int status = -1;
            if (info.dli_sname[0] == '_')
            {
#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
                demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
#endif
            }

            const std::string function_name = (status == 0 ? demangled : info.dli_sname == nullptr ? symbols[i] : info.dli_sname);

            snprintf(buf, sizeof(buf), "%-3d %*p %s + %zd\n",
                     i, int(2 + sizeof(void*) * 2), callstack[i],
                     function_name.c_str(),
                     (char*) callstack[i] - (char*) info.dli_saddr);
            //std::cout << buf << std::endl;

            json entry;
            entry["function"] = function_name;

            if (not function_name.empty())
            {
                if (function_name.substr(0, 5) == "std::" or function_name.substr(0, 2) == "__")
                {
                    entry["in_app"] = false;
                }
            }

            result.push_back(entry);

            free(demangled);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%-3d %*p %s\n",
                     i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
        }
    }
    free(symbols);
#endif

    return result;
}


template<typename T>
const char* pretty_typename(const T& e)
{
    const char* mangled_name = typeid(e).name();
#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
    int status;
    return abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
#else
    return mangled_name;
#endif
}

template<typename T>
std::string get_module(const T& e)
{
    std::string result = pretty_typename(e);
    return result.substr(0, result.find_first_of(':'));
}

/*!
 * @brief get the seconds since epoch
 * @return seconds since epoch
 */
std::int64_t get_timestamp()
{
    const auto tp = std::chrono::system_clock::now();
    const auto dur = tp.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(dur).count();
}

/*!
 * @brief get the current date and time as ISO 8601 string
 * @return ISO 8601 string
 */
std::string get_iso8601()
{
    std::time_t now;
    std::time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
    return buf;
}

/*!
 * @brief generate a UUID4 without dashes
 * @return UUID4
 */
std::string generate_uuid()
{
    std::random_device random_device;
    std::default_random_engine random_engine(random_device());
    std::uniform_int_distribution<int> uniform_dist(0, 15);

    std::string result(32, ' ');

    for (size_t i = 0; i < 32; ++i)
    {
        if (i == 12)
        {
            result[i] = '4';
        }
        else
        {
            const auto r = uniform_dist(random_engine);
            if (r < 10)
            {
                result[i] = '0' + r;
            }
            else
            {
                result[i] = 'a' + static_cast<char>(r - 10);
            }
        }
    }

    return result;
}

}

/*!
 * @brief a C++ client for Sentry
 */
class crow
{
  public:
    /*!
     * @brief create a client
     *
     * @param[in] dsn the DNS string
     * @param[in] attributes an optional attributes object
     * @param[in] install_handlers whether to install a termination handler
     *
     * @throw std::invalid_argument if DNS string is invalid
     *
     * @note If @a dns is empty, the client is disabled.
     *
     * @note
     * In case @a install_handlers is set to `true` (default), the currently
     * installed termination handler is replaced by a new termination handler
     * that first reports possibly uncaught exceptions and then executes the
     * previously installed termination handler. Note that termination
     * handlers installed after creating this client would override this
     * termination behavior.
     */
    explicit crow(const std::string& dsn,
                  const json& attributes = nullptr,
                  const bool install_handlers = true)
        : m_enabled(not dsn.empty()),
          m_payload({{"platform", "c"}, {"sdk", {{"name", "crow"}, {"version", NLOHMANN_CROW_VERSION}}}})
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
        m_payload["contexts"]["os"]["build"] = NLOHMANN_CROW_OS_VERSION;
        if (not std::string(NLOHMANN_CROW_UNAME).empty())
        {
            m_payload["contexts"]["os"]["kernel_version"] = NLOHMANN_CROW_UNAME;
        }
        else if (not std::string(NLOHMANN_CROW_VER).empty())
        {
            m_payload["contexts"]["os"]["kernel_version"] = NLOHMANN_CROW_VER;
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
            m_payload["contexts"]["user"]["id"] = std::string(user) + "@" + NLOHMANN_CROW_HOSTNAME;
            m_payload["contexts"]["user"]["username"] = user;
        }

        // add given attributes
        if (attributes.is_object())
        {
            m_payload["contexts"].update(attributes);
        }

        // install termination handler
        if (install_handlers)
        {
            existing_termination_handler = std::set_terminate([]() {});
            std::set_terminate(&new_termination_handler);
            detail::last = this;
        }
    }

    /*!
     * @brief copy constructor
     * @param other client to copy
     * @note The last event id is not preserved by copying.
     */
    crow(const crow& other)
        : m_enabled(other.m_enabled),
          m_public_key(other.m_public_key),
          m_secret_key(other.m_secret_key),
          m_store_url(other.m_store_url),
          m_payload(other.m_payload)
    {}

    /*!
     * @brief destructor
     * @note Waits until pending HTTP requests complete
     */
    ~crow()
    {
        if (m_pending_future.valid())
        {
            m_pending_future.wait();
        }
    }

    /*!
     * @brief capture a message
     *
     * @param[in] message the message to capture
     * @param[in] options an optional options object
     * @param[in] asynchronous whether the message should be sent asynchronously
     *
     * @return id of the captured message
     */
    void capture_message(const std::string& message,
                         const json& options = nullptr,
                         const bool asynchronous = true)
    {
        m_payload["message"] = message;
        m_payload["event_id"] = nlohmann::detail::generate_uuid();
        m_payload["timestamp"] = nlohmann::detail::get_iso8601();

        if (options.is_object())
        {
            m_payload.update(options);
        }

        m_pending_future = std::async(std::launch::async, [this] { return post(m_payload); });
        if (not asynchronous)
        {
            m_pending_future.wait();
        }
    }

    /*!
     * @brief capture an exception
     *
     * @param[in] exception the passed exception
     * @param[in] asynchronous whether the message should be sent asynchronously
     * @param[in] handled whether the exception was handled and only reported
     *
     * @return id of the captured exception
     */
    void capture_exception(const std::exception& exception,
                           const bool asynchronous = true,
                           const bool handled = true)
    {
        std::stringstream thread_id;
        thread_id << std::this_thread::get_id();
        m_payload["exception"].push_back({{"type", detail::pretty_typename(exception)},
            {"value", exception.what()},
            {"module", detail::get_module(exception)},
            {"mechanism", {{"handled", handled}, {"description", handled ? "handled exception" : "unhandled exception"}}},
            {"stacktrace", {{"frames", detail::get_backtrace()}}},
            {"thread_id", thread_id.str()}});
        m_payload["event_id"] = detail::generate_uuid();
        m_payload["timestamp"] = nlohmann::detail::get_iso8601();

        m_pending_future = std::async(std::launch::async, [this] { return post(m_payload); });
        if (not asynchronous)
        {
            m_pending_future.wait();
        }
    }

    /*!
     * @brief add a breadcrumb to the current context
     *
     * @param[in] message message for the breadcrumb
     * @param[in] type type of the breadcrumb (optional)
     * @param[in] data additional JSON object (optional)
     */
    void add_breadcrumb(const std::string& message,
                        const std::string& type = "default",
                        const json& data = nullptr)
    {
        json breadcrumb =
        {
            {"event_id", detail::generate_uuid()},
            {"message", message},
            {"type", type},
            {"timestamp", detail::get_timestamp()}
        };

        if (not data.is_null())
        {
            breadcrumb["data"] = data;
        }

        m_payload["breadcrumbs"]["values"].push_back(std::move(breadcrumb));
    }

    /*!
     * @brief return the id of the last reported event
     * @return event id, or empty string, if no request has been made
     */
    std::string get_last_event_id() const
    {
        if (m_pending_future.valid())
        {
            return json::parse(m_pending_future.get()).at("id");
        }
        else
        {
            return "";
        }
    }

  private:
    /*!
     * @brief POST the payload to the Sentry sink URL
     *
     * @param payload payload to send
     *
     * @return result
     */
    std::string post(const json& payload)
    {
        if (m_enabled)
        {
            curl_wrapper curl;

            // add security header
            std::string security_header = "X-Sentry-Auth: Sentry sentry_version=5,sentry_client=crow/";
            security_header += std::string(NLOHMANN_CROW_VERSION) + ",sentry_timestamp=";
            security_header += std::to_string(detail::get_timestamp());
            security_header += ",sentry_key=" + m_public_key;
            security_header += ",sentry_secret=" + m_secret_key;
            curl.set_header(security_header.c_str());

            return curl.post(m_store_url, payload);
        }

        return "";
    }

    /*!
     * @brief termination handler that detects uncaught exceptions
     *
     * @post previously installed termination handler is executed
     */
    static void new_termination_handler()
    {
        auto current_ex = std::current_exception();
        if (current_ex)
        {
            assert(detail::last != nullptr);
            detail::last->add_breadcrumb("uncaught exception", "default", {{"level", "critical"}});
            try
            {
                std::rethrow_exception(current_ex);
            }
            catch (const std::exception& e)
            {
                detail::last->capture_exception(e, false, false);
            }
        }

        detail::last->existing_termination_handler();
    }

  private:
    /// whether the client is enabled
    const bool m_enabled = true;
    /// the public key to be used in requests
    std::string m_public_key;
    /// the secret key to be used in requests
    std::string m_secret_key;
    /// the URL to send events to
    std::string m_store_url;
    /// the payload of all events
    json m_payload = {};
    /// the result of the last HTTP POST
    mutable std::future<std::string> m_pending_future;
    /// the termination handler installed before initializing the client
    std::terminate_handler existing_termination_handler = nullptr;
};

}

using crow = nlohmann::crow;

#endif
