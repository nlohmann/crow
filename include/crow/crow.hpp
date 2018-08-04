/*
 _____ _____ _____ _ _ _
|     | __  |     | | | |  Crow - a Sentry client for C++
|   --|    -|  |  | | | |  version 0.0.3
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
#include <mutex>
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

/*!
 * @brief return pretty type name
 * @param[in] type_id_name result of type_id().name()
 * @param[in] only_module whether only the module name should be returned
 * @return demangled prettified name
 */
std::string pretty_name(const char* type_id_name,
                        const bool only_module = false)
{
#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
    int status;
    std::string result = abi::__cxa_demangle(type_id_name, nullptr, nullptr, &status);
#else
    std::string result = type_id_name;
#endif

    return only_module
           ? result.substr(0, result.find_first_of(':'))
           : result;
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
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now));
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

    for (std::size_t i = 0; i < result.size(); ++i)
    {
        // the 12th character must be a '4'
        if (i == 12)
        {
            result[i] = '4';
        }
        else
        {
            // get a random number from 0..15
            const auto r = static_cast<char>(uniform_dist(random_engine));
            if (r < 10)
            {
                result[i] = '0' + r;
            }
            else
            {
                result[i] = 'a' + r - static_cast<char>(10);
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
     * @param[in] context an optional attributes object
     * @param[in] install_handlers whether to install a termination handler
     *
     * @throw std::invalid_argument if DNS string is invalid
     * @throw std::invalid_argument if context object contains invalid key
     *
     * @note If @a dns is empty, the client is disabled.
     *
     * @note
     * In case @a install_handlers is set to `true` (default), the currently
     * installed termination handler is replaced by a new termination handler
     * that first reports possibly uncaught exceptions and then executes the
     * previously installed termination handler. Note that termination
     * handlers installed after creating this client would override this
     * termination behavior. The termination handler can be installed later
     * with function @ref install_handler().
     *
     * @since 0.0.1
     */
    explicit crow(const std::string& dsn,
                  const json& context = nullptr,
                  const bool install_handlers = true)
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

    /*!
     * @brief install termination handler to handle uncaught exceptions
     * @post uncaught exceptions are reported prior to executing existing termination handler
     *
     * @since 0.0.3
     */
    void install_handler()
    {
        if (existing_termination_handler == nullptr)
        {
            existing_termination_handler = std::set_terminate([]() {});
            std::set_terminate(&new_termination_handler);
            detail::last = this;
        }
    }

    /*!
     * @brief copy constructor
     *
     * @param[in] other client to copy
     *
     * @note The last event id is not preserved by copying.
     *
     * @since 0.0.2
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
     *
     * @note Waits until pending HTTP requests complete.
     *
     * @since 0.0.2
     */
    ~crow()
    {
        if (m_pending_future.valid())
        {
            m_pending_future.wait();
        }
    }
    /*!
     * @name event capturing
     * @{
     */

    /*!
     * @brief capture a message
     *
     * @param[in] message the message to capture
     * @param[in] context an optional context object
     * @param[in] asynchronous whether the message should be sent asynchronously
     *
     * @throw std::invalid_argument if context object contains invalid key
     *
     * @since 0.0.1
     */
    void capture_message(const std::string& message,
                         const json& context = nullptr,
                         const bool asynchronous = true)
    {
        std::lock_guard<std::mutex> lock(m_payload_mutex);
        m_payload["message"] = message;
        m_payload["event_id"] = nlohmann::detail::generate_uuid();
        m_payload["timestamp"] = nlohmann::detail::get_iso8601();

        // add given context
        merge_context(context);

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
     * @param[in] context an optional context object
     * @param[in] asynchronous whether the message should be sent asynchronously
     * @param[in] handled whether the exception was handled and only reported
     *
     * @throw std::invalid_argument if context object contains invalid key
     *
     * @since 0.0.1, context added with 0.0.3
     */
    void capture_exception(const std::exception& exception,
                           const json& context = nullptr,
                           const bool asynchronous = true,
                           const bool handled = true)
    {
        std::stringstream thread_id;
        thread_id << std::this_thread::get_id();
        std::lock_guard<std::mutex> lock(m_payload_mutex);
        m_payload["exception"].push_back({{"type", detail::pretty_name(typeid(exception).name())},
            {"value", exception.what()},
            {"module", detail::pretty_name(typeid(exception).name(), true)},
            {"mechanism", {{"handled", handled}, {"description", handled ? "handled exception" : "unhandled exception"}}},
            {"stacktrace", {{"frames", detail::get_backtrace()}}},
            {"thread_id", thread_id.str()}});
        m_payload["event_id"] = detail::generate_uuid();
        m_payload["timestamp"] = nlohmann::detail::get_iso8601();

        // add given context
        merge_context(context);

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
     *
     * @since 0.0.1
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

        std::lock_guard<std::mutex> lock(m_payload_mutex);
        m_payload["breadcrumbs"]["values"].push_back(std::move(breadcrumb));
    }

    /*!
     * @brief return the id of the last reported event
     *
     * @return event id, or empty string, if no request has been made
     *
     * @since 0.0.2
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

    /*!
     * @}
     */

    /*!
     * @name context management
     * @{
     */

    /*!
     * @brief return current context
     *
     * @return current context
     *
     * @since 0.0.3
     */
    const json& get_context() const
    {
        return m_payload;
    }

    /*!
     * @brief add elements to the "user" context for future events
     *
     * @param[in] data data to add to the extra context
     *
     * @since 0.0.3
     */
    void add_user_context(const json& data)
    {
        std::lock_guard<std::mutex> lock(m_payload_mutex);
        m_payload["user"].update(data);
    }

    /*!
     * @brief add elements to the "tags" context for future events
     *
     * @param[in] data data to add to the extra context
     *
     * @since 0.0.3
     */
    void add_tags_context(const json& data)
    {
        std::lock_guard<std::mutex> lock(m_payload_mutex);
        m_payload["tags"].update(data);
    }

    /*!
     * @brief add elements to the "request" context for future events
     *
     * @param[in] data data to add to the extra context
     *
     * @since 0.0.3
     */
    void add_request_context(const json& data)
    {
        std::lock_guard<std::mutex> lock(m_payload_mutex);
        m_payload["request"].update(data);
    }

    /*!
     * @brief add elements to the "extra" context for future events
     *
     * @param[in] data data to add to the extra context
     *
     * @since 0.0.3
     */
    void add_extra_context(const json& data)
    {
        std::lock_guard<std::mutex> lock(m_payload_mutex);
        m_payload["extra"].update(data);
    }

    /*!
     * @brief add context information to payload for future events
     *
     * @param[in] context the context to add
     *
     * @note @a context must be an object; allowed keys are "user", "request", "extea", or "tags"
     * @throw std::invalid_argument if context object contains invalid key
     *
     * @since 0.0.3
     */
    void merge_context(const json& context)
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

    /*!
     * @brief reset context for future events
     *
     * @post context is in the same state as it was after construction
     *
     * @since 0.0.3
     */
    void clear_context()
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

    /*!
     * @}
     */

  private:
    /*!
     * @brief POST the payload to the Sentry sink URL
     *
     * @param[in] payload payload to send
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
                detail::last->capture_exception(e, nullptr, false, false);
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
    /// a mutex to make payload thread-safe
    std::mutex m_payload_mutex;
};

}

using crow = nlohmann::crow;

#endif
