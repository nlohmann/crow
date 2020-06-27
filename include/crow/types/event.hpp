#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <crow/types/helpers/base_types.hpp>
#include <crow/types/helpers/helpers.hpp>
#include <crow/types/interfaces/breadcrumbs.hpp>
#include <crow/types/interfaces/context.hpp>
#include <crow/types/interfaces/exception.hpp>
#include <crow/types/interfaces/gpu.hpp>
#include <crow/types/interfaces/http.hpp>
#include <crow/types/interfaces/sdk.hpp>
#include <crow/types/interfaces/stacktrace.hpp>
#include <crow/types/interfaces/template.hpp>
#include <crow/types/interfaces/thread.hpp>
#include <crow/types/interfaces/user.hpp>

namespace crow {
namespace types {

enum class platform_t
{
    as3,
    c,
    cfml,
    cocoa,
    csharp,
    go,
    java,
    javascript,
    native,
    node,
    objc,
    other,
    perl,
    php,
    python,
    ruby
};

void to_json(nlohmann::json& j, const platform_t& platform);

class event_t
{
  public:
    event_t();
    event_t(const event_t&) = default;
    event_t(event_t&&) = default;
    event_t(const std::exception& exception, bool handled = true);

    /// Hexadecimal string representing a uuid4 value. The length is exactly 32 characters. Dashes are not allowed.
    std::string event_id;

    /// Indicates when the logging record was created (in the Sentry SDK). The Sentry server assumes the time is in UTC. The timestamp should be in ISO 8601 format, without a timezone.
    std::string timestamp;

    std::string message;

    /// The name of the logger which created the record.
    std::string logger;

    /// A string representing the platform the SDK is submitting from. This will be used by the Sentry interface to customize various components in the interface.
    platform_t platform{platform_t::c};

    /// Information about the SDK sending the event. Note that the integrations key is optional and used to list any of the SDK and language specific integrations that the user is actively using.
    interfaces::sdk_t sdk;

    /// The record severity. Defaults to error.
    level_t level{level_t::error};

    /// The name of the transaction which caused this exception.
    std::string transaction;

    /// Identifies the host SDK from which the event was recorded.
    std::string server_name;

    /// The release version of the application. This value will generally be something along the lines of the git SHA for the given project.
    std::string release;

    /// The distribution of the application. Distributions are used to disambiguate build or deployment variants of the same release of an application. For example, the dist can be the build number of an XCode build or the version code of an Android build.
    std::string dist;

    /// A map or list of tags for this event.
    std::unordered_map<std::string, std::string> tags;

    /// The environment name, such as "production" or "staging".
    std::string environment;

    /// A list of relevant modules and their versions.
    std::unordered_map<std::string, std::string> modules;

    /// An arbitrary mapping of additional metadata to store with the event.
    nlohmann::json extra;

    /// A list of strings used to dictate the deduplication of this event. A value of {{ default }} will be replaced with the built-in behavior, thus allowing you to extend it, or completely replace it.
    std::vector<std::string> fingerprint;

    // interfaces

    interfaces::exception_t exception;
    interfaces::stacktrace_t stacktrace;
    interfaces::template_t template_;
    interfaces::breadcrumbs_t breadcrumbs;
    interfaces::context_t context;
    interfaces::gpu_t gpu;
    interfaces::http_t http;
    interfaces::thread_t threads;
    interfaces::user_t user;

  private:
    void set_default();
};

void to_json(nlohmann::json& j, const event_t& event);

} // namespace types
} // namespace crow
