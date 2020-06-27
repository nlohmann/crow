#include <crow/types/interfaces/context.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const browser_t& browser)
{
    fill_if_not_empty("name", browser.name, j);
    fill_if_not_empty("version", browser.version, j);
}

void to_json(nlohmann::json& j, const app_t& app)
{
    fill_if_not_empty("app_start_time", app.app_start_time, j);
    fill_if_not_empty("device_app_hash", app.device_app_hash, j);
    fill_if_not_empty("build_type", app.build_type, j);
    fill_if_not_empty("app_identifier", app.app_identifier, j);
    fill_if_not_empty("app_name", app.app_name, j);
    fill_if_not_empty("app_version", app.app_version, j);
    fill_if_not_empty("app_build", app.app_build, j);
}

void to_json(nlohmann::json& j, const runtime_t& runtime)
{
    fill_if_not_empty("name", runtime.name, j);
    fill_if_not_empty("version", runtime.version, j);
    fill_if_not_empty("raw_description", runtime.raw_description, j);
}

void to_json(nlohmann::json& j, const os_t& os)
{
    fill_if_not_empty("name", os.name, j);
    fill_if_not_empty("version", os.version, j);
    fill_if_not_empty("build", os.build, j);
    fill_if_not_empty("kernel_version", os.kernel_version, j);
    fill_if_not_empty("rooted", os.rooted, j);
    fill_if_not_empty("raw_description", os.raw_description, j);
}

void to_json(nlohmann::json& j, const device_t& device)
{
    fill_if_not_empty("name", device.name, j);
    fill_if_not_empty("family", device.family, j);
    fill_if_not_empty("model", device.model, j);
    fill_if_not_empty("model_id", device.model_id, j);
    fill_if_not_empty("arch", device.arch, j);
    fill_if_not_empty("battery_level", device.battery_level, j);
    fill_if_not_empty("orientation", device.orientation, j);
    fill_if_not_empty("manufacturer", device.manufacturer, j);
    fill_if_not_empty("brand", device.brand, j);
    fill_if_not_empty("screen_resolution", device.screen_resolution, j);
    fill_if_not_empty("screen_density", device.screen_density, j);
    fill_if_not_empty("screen_dpi", device.screen_dpi, j);
    fill_if_not_empty("online", device.online, j);
    fill_if_not_empty("charging", device.charging, j);
    fill_if_not_empty("low_memory", device.low_memory, j);
    fill_if_not_empty("simulator", device.simulator, j);
    fill_if_not_empty("memory_size", device.memory_size, j);
    fill_if_not_empty("free_memory", device.free_memory, j);
    fill_if_not_empty("usable_memory", device.usable_memory, j);
    fill_if_not_empty("storage_size", device.storage_size, j);
    fill_if_not_empty("free_storage", device.free_storage, j);
    fill_if_not_empty("external_storage_size", device.external_storage_size, j);
    fill_if_not_empty("external_free_storage", device.external_free_storage, j);
    fill_if_not_empty("boot_time", device.boot_time, j);
    fill_if_not_empty("timezone", device.timezone, j);
}

void to_json(nlohmann::json& j, const context_t& context)
{
    fill_if_not_empty("device", nlohmann::json(context.device), j);
    fill_if_not_empty("os", nlohmann::json(context.os), j);
    fill_if_not_empty("runtime", nlohmann::json(context.runtime), j);
    fill_if_not_empty("app", nlohmann::json(context.app), j);
    fill_if_not_empty("browser", nlohmann::json(context.browser), j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
