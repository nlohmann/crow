#include <crow/types/interfaces/sdk.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const package_t& package)
{
    fill_if_not_empty("name", package.name, j);
    fill_if_not_empty("version", package.version, j);
}

void to_json(nlohmann::json& j, const sdk_t& sdk)
{
    fill_if_not_empty("name", sdk.name, j);
    fill_if_not_empty("version", sdk.version, j);
    fill_if_not_empty("integrations", sdk.integrations, j);
    fill_if_not_empty("packages", sdk.packages, j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
