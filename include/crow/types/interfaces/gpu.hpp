#pragma once

#include <string>
#include <crow/types/helpers/base_types.hpp>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

/*!
 * @brief GPU Interface
 *
 * An interface which describes the main GPU of the device.
 *
 * @sa https://docs.sentry.io/development/sdk-dev/interfaces/#gpu-interface
 */
class gpu_t
{
  public:
    /// the name of the graphics device
    std::string name;

    /// the Version of graphics device
    std::string version;

    /// the PCI Id of the graphics device
    std::string id;

    /// the PCI vendor Id of the graphics device
    std::string vendor_id;

    /// the vendor name as reported by the graphics device
    std::string vendor_name;

    /// the total GPU memory available in Megabytes
    int memory_size{-1};

    /// the device low level API type
    std::string api_type;

    /// whether the GPU has multi-threaded rendering or not
    optional_bool multi_threaded_rendering;

    /// the Non-Power-Of-Two-Support support
    std::string npot_support;
};

void to_json(nlohmann::json& j, const gpu_t& gpu);

} // namespace interfaces
} // namespace types
} // namespace crow
