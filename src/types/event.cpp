#include <cstdlib>
#include <crow/crow_config.hpp>
#include <crow/types/event.hpp>
#include <crow/utils/utils.hpp>

namespace crow {
namespace types {

event_t::event_t()
{
    set_default();
}

void to_json(nlohmann::json& j, const platform_t& platform)
{
    switch (platform)
    {
        case platform_t::as3:
            j = "as3";
            break;
        case platform_t::c:
            j = "c";
            break;
        case platform_t::cfml:
            j = "cfml";
            break;
        case platform_t::cocoa:
            j = "cocoa";
            break;
        case platform_t::csharp:
            j = "csharp";
            break;
        case platform_t::go:
            j = "go";
            break;
        case platform_t::java:
            j = "java";
            break;
        case platform_t::javascript:
            j = "javascript";
            break;
        case platform_t::native:
            j = "native";
            break;
        case platform_t::node:
            j = "node";
            break;
        case platform_t::objc:
            j = "objc";
            break;
        case platform_t::other:
            j = "other";
            break;
        case platform_t::perl:
            j = "perl";
            break;
        case platform_t::php:
            j = "php";
            break;
        case platform_t::python:
            j = "python";
            break;
        case platform_t::ruby:
            j = "ruby";
            break;
    }
}

void event_t::set_default()
{
    platform = platform_t::c;

    /////////////////////////////////////////////////////////////////////////
    // SDK
    /////////////////////////////////////////////////////////////////////////

    sdk.name = NLOHMANN_CROW_PACKAGE_NAME;
    sdk.version = std::string(NLOHMANN_CROW_VERSION) + " " + NLOHMANN_CROW_CURL_WRAPPER_VERSION;

    sdk.packages.push_back({nlohmann::json::meta()["name"], nlohmann::json::meta()["version"]["string"]});

#ifdef NLOHMANN_CROW_HAVE_BACKTRACE
    sdk.integrations.emplace_back("backtrace");
#endif
#ifdef NLOHMANN_CROW_HAVE_CXA_DEMANGLE
    sdk.integrations.emplace_back("demangle");
#endif

    /////////////////////////////////////////////////////////////////////////
    // App
    /////////////////////////////////////////////////////////////////////////

    context.app.build_type = NLOHMANN_CROW_CMAKE_BUILD_TYPE;
    context.app.app_name = NLOHMANN_CROW_CMAKE_PROJECT_NAME;
    context.app.app_version = NLOHMANN_CROW_CMAKE_PROJECT_VERSION;
    extra["PID"] = utils::get_process_id();
    extra["environment"] = utils::get_environment_variables();

    /////////////////////////////////////////////////////////////////////////
    // Device
    /////////////////////////////////////////////////////////////////////////

    context.device.name = utils::get_environment_variable("NLOHMANN_CROW_HOSTNAME");
    if (context.device.name.empty())
    {
        context.device.name = NLOHMANN_CROW_HOSTNAME;
    }

    context.device.arch = NLOHMANN_CROW_CMAKE_SYSTEM_PROCESSOR;
    context.device.family = NLOHMANN_CROW_SYS_PRODUCT_FAMILY;
    context.device.manufacturer = NLOHMANN_CROW_SYS_VENDOR;

    context.device.model = NLOHMANN_CROW_SYSCTL_HW_MODEL;
    if (context.device.model.empty())
    {
        context.device.model = NLOHMANN_CROW_SYS_PRODUCT_NAME;
    }

    context.device.memory_size = NLOHMANN_CROW_TOTAL_PHYSICAL_MEMORY;
    context.device.usable_memory = NLOHMANN_CROW_AVAILABLE_PHYSICAL_MEMORY;

    context.runtime.name = NLOHMANN_CROW_CMAKE_CXX_COMPILER_ID;
    context.runtime.version = NLOHMANN_CROW_CMAKE_CXX_COMPILER_VERSION;
    context.runtime.raw_description = NLOHMANN_CROW_CXX;

    /////////////////////////////////////////////////////////////////////////
    // OS
    /////////////////////////////////////////////////////////////////////////

    context.os.name = utils::get_environment_variable("NLOHMANN_CROW_CMAKE_SYSTEM_NAME");
    if (context.os.name.empty())
    {
        context.os.name = NLOHMANN_CROW_CMAKE_SYSTEM_NAME;
    }

    context.os.version = NLOHMANN_CROW_OS_RELEASE;

    context.os.version = NLOHMANN_CROW_OS_VERSION;
    if (context.os.build.empty())
    {
        context.os.build = NLOHMANN_CROW_CMAKE_SYSTEM_VERSION;
    }

    context.os.kernel_version = utils::get_environment_variable("NLOHMANN_CROW_UNAME");
    if (context.os.kernel_version.empty())
    {
        context.os.kernel_version = NLOHMANN_CROW_UNAME;
        if (context.os.kernel_version.empty())
        {
            context.os.kernel_version = NLOHMANN_CROW_SYSTEMINFO;
        }
    }

    /////////////////////////////////////////////////////////////////////////
    // User
    /////////////////////////////////////////////////////////////////////////

    auto user_name = utils::get_environment_variable("USER");
    if (user_name.empty())
    {
        user_name = utils::get_environment_variable("USERNAME");
        if (user_name.empty())
        {
            user_name = NLOHMANN_CROW_WHOAMI;
        }
    }

    if (not user_name.empty())
    {
        user.id = user_name + "@" + context.device.name;
        user.username = user_name;
    }

    user.ip_address = utils::get_environment_variable("NLOHMANN_CROW_IP_ADDRESS");
    if (user.ip_address.empty())
    {
        user.ip_address = NLOHMANN_CROW_IP_ADDRESS;
    }
}
event_t::event_t(const std::exception& exception, bool handled)
    : exception(interfaces::exception_t(exception, handled))
    , level(handled ? types::level_t::error : types::level_t::fatal)
{}

void to_json(nlohmann::json& j, const event_t& event)
{
    fill_if_not_empty("event_id", event.event_id, j);
    fill_if_not_empty("timestamp", event.timestamp, j);
    fill_if_not_empty("message", event.message, j);
    fill_if_not_empty("logger", event.logger, j);
    fill_if_not_empty("platform", nlohmann::json(event.platform), j);
    fill_if_not_empty("sdk", nlohmann::json(event.sdk), j);
    j["level"] = nlohmann::json(event.level);
    fill_if_not_empty("transaction", event.transaction, j);
    fill_if_not_empty("server_name", event.server_name, j);
    fill_if_not_empty("release", event.release, j);
    fill_if_not_empty("dist", event.dist, j);
    fill_if_not_empty("tags", event.tags, j);
    fill_if_not_empty("environment", event.environment, j);
    fill_if_not_empty("modules", event.modules, j);
    fill_if_not_empty("extra", event.extra, j);
    fill_if_not_empty("fingerprint", event.fingerprint, j);
    fill_if_not_empty("exception", nlohmann::json(event.exception), j);
    fill_if_not_empty("stacktrace", nlohmann::json(event.stacktrace), j);
    fill_if_not_empty("template", nlohmann::json(event.template_), j);
    fill_if_not_empty("breadcrumbs", nlohmann::json(event.breadcrumbs), j);
    fill_if_not_empty("contexts", nlohmann::json(event.context), j);
    fill_if_not_empty("gpu", nlohmann::json(event.gpu), j);
    fill_if_not_empty("request", nlohmann::json(event.http), j);
    fill_if_not_empty("threads", nlohmann::json(event.threads), j);
    fill_if_not_empty("user", nlohmann::json(event.user), j);
}

} // namespace types
} // namespace crow
