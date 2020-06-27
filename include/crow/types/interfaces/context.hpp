#pragma once

#include <string>
#include <crow/types/helpers/base_types.hpp>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

/*!
 * Carries information about the browser or user agent for web-related errors. This can either be the browser this event ocurred in, or the user agent of a web request that triggered the event.
 */
class browser_t
{
  public:
    /// display name of the browser application
    std::string name;

    /// version string of the browser
    std::string version;
};

void to_json(nlohmann::json& j, const browser_t& browser);

/*!
 * Describes the application. As opposed to the runtime, this is the actual application that was running and carries meta data about the current session.
 */
class app_t
{
  public:
    /// Optional. Formatted UTC timestamp when the application was started by the user.
    std::string app_start_time;

    /// Optional. Application specific device identifier.
    std::string device_app_hash;

    /// Optional. String identifying the kind of build, e.g. testflight.
    std::string build_type;

    /// Optional. Version-independent application identifier, often a dotted bundle ID.
    std::string app_identifier;

    /// Optional. Human readable application name, as it appears on the platform.
    std::string app_name;

    /// Optional. Human readable application version, as it appears on the platform.
    std::string app_version;

    /// Optional. Internal build identifier, as it appears on the platform.
    std::string app_build;
};

void to_json(nlohmann::json& j, const app_t& app);

/*!
 * Describes a runtime in more detail. Typically this context is used multiple times if multiple runtimes are involved (for instance if you have a JavaScript application running on top of JVM)
 */
class runtime_t
{
  public:
    /// the name of the runtime
    std::string name;

    /// the version identifier of the runtime
    std::string version;

    /// an unprocessed description string obtained by the runtime
    std::string raw_description;
};

void to_json(nlohmann::json& j, const runtime_t& runtime);

class os_t
{
  public:
    /// the name of the operating system
    std::string name;

    /// the version of the operating system
    std::string version;

    /// the internal build revision of the operating system
    std::string build;

    /// an independent kernel version string
    std::string kernel_version;

    /// a flag indicating whether the OS has been jailbroken or rooted
    optional_bool rooted;

    /// an unprocessed description string obtained by the operating system
    std::string raw_description;
};

void to_json(nlohmann::json& j, const os_t& os);

/*!
 * This describes the device that caused the event. This is most appropriate for mobile applications.
 */
class device_t
{
  public:
    /// Optional. The name of the device. This is typically a hostname.
    std::string name;

    /// Optional. The family of the device. This is normally the common part of model names across generations. For instance iPhone would be a reasonable family, so would be Samsung Galaxy.
    std::string family;

    /// Optional. The model name. This for instance can be Samsung Galaxy S3.
    std::string model;

    /// Optional. An internal hardware revision to identify the device exactly.
    std::string model_id;

    /// Optional. The CPU architecture.
    std::string arch;

    /// Optional. If the device has a battery, this can be an floating point value defining the battery level (in the range 0-100).
    double battery_level{-1};

    /// Optional. This can be a string portrait or landscape to define the orientation of a device.
    std::string orientation;

    /// Optional. The manufacturer of the device.
    std::string manufacturer;

    /// Optional. The brand of the device.
    std::string brand;

    /// Optional. The screen resolution. (e.g.: 800x600, 3040x1444).
    std::string screen_resolution;

    /// Optional. A floating point denoting the screen density.
    double screen_density{-1};

    /// Optional. A decimal value reflecting the DPI (dots-per-inch) density.
    int screen_dpi{-1};

    /// Optional. Whether the device was online or not.
    optional_bool online;

    /// Optional. Whether the device was charging or not.
    optional_bool charging;

    /// Optional. Whether the device was low on memory.
    optional_bool low_memory;

    /// Optional. A flag indicating whether this device is a simulator or an actual device.
    optional_bool simulator;

    /// Optional. Total system memory available in bytes.
    long memory_size{-1};

    /// Optional. Free system memory in bytes.
    long free_memory{-1};

    /// Optional. Memory usable for the app in bytes.
    long usable_memory{-1};

    /// Optional. Total device storage in bytes.
    long storage_size{-1};

    /// Optional. Free device storage in bytes.
    long free_storage{-1};

    /// Optional. Total size of an attached external storage in bytes (e.g.: android SDK card).
    long external_storage_size{-1};

    /// Optional. Free size of an attached external storage in bytes (e.g.: android SDK card).
    long external_free_storage{-1};

    /// Optional. A formatted UTC timestamp when the system was booted, e.g.: "2018-02-08T12:52:12Z".
    std::string boot_time;

    /// Optional. The timezone of the device, e.g.: Europe/Vienna.
    std::string timezone;
};

void to_json(nlohmann::json& j, const device_t& device);
/*!
 * @brief Contexts Interface
 *
 * The context interfaces provide additional context data. Typically this is data related to the current user, the current HTTP request.
 *
 * @sa https://docs.sentry.io/development/sdk-dev/interfaces/#contexts-interface
 */
class context_t
{
  public:
    /// the device that caused the event
    device_t device;

    /// the operating system on which the event was created
    os_t os;

    /// describes a runtime in more detail
    runtime_t runtime;

    /// describes the application
    app_t app;

    /// carries information about the browser or user agent for web-related errors
    browser_t browser;
};

void to_json(nlohmann::json& j, const context_t& context);

} // namespace interfaces
} // namespace types
} // namespace crow
