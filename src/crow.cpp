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
#include <crow/crow.hpp>
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

namespace nlohmann
{

namespace detail
{
crow* last = nullptr;

// https://gist.github.com/fmela/591333
// This function produces a stack backtrace with demangled function & method names.
json get_backtrace(int skip)
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

std::string pretty_name(const char* type_id_name,
                        const bool only_module)
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

std::int64_t get_timestamp()
{
    const auto tp = std::chrono::system_clock::now();
    const auto dur = tp.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(dur).count();
}

std::string get_iso8601()
{
    std::time_t now;
    std::time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now));
    return buf;
}

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
        detail::last = this;
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
    m_payload["event_id"] = nlohmann::detail::generate_uuid();
    m_payload["timestamp"] = nlohmann::detail::get_iso8601();

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

void crow::add_breadcrumb(const std::string& message,
                          const json& attributes)
{
    json breadcrumb =
    {
        {"event_id", detail::generate_uuid()},
        {"message", message},
        {"level", "info"},
        {"type", "default"},
        {"category", "log"},
        {"timestamp", detail::get_timestamp()}
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
        return json::parse(m_pending_future.get()).at("id");
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
        security_header += std::to_string(detail::get_timestamp());
        security_header += ",sentry_key=" + m_public_key;
        security_header += ",sentry_secret=" + m_secret_key;
        curl.set_header(security_header.c_str());

        return curl.post(m_store_url, payload);
    }

    return "";
}

void crow::new_termination_handler()
{
    auto current_ex = std::current_exception();
    if (current_ex)
    {
        assert(detail::last != nullptr);
        detail::last->add_breadcrumb("uncaught exception", {{"type", "exceptiomn"}, {"level", "critical"}});
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

}