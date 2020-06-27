#include <crow/types/interfaces/gpu.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const gpu_t& gpu)
{
    fill_if_not_empty("name", gpu.name, j);
    fill_if_not_empty("version", gpu.version, j);
    fill_if_not_empty("id", gpu.id, j);
    fill_if_not_empty("vendor_id", gpu.vendor_id, j);
    fill_if_not_empty("vendor_name", gpu.vendor_name, j);
    fill_if_not_empty("memory_size", gpu.memory_size, j);
    fill_if_not_empty("api_type", gpu.api_type, j);
    fill_if_not_empty("multi_threaded_rendering", gpu.multi_threaded_rendering, j);
    fill_if_not_empty("npot_support", gpu.npot_support, j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
