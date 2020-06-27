#pragma once

#include <string>
#include <vector>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

class package_t
{
  public:
    /// name of the package
    std::string name;

    /// a semver version
    std::string version;
};

void to_json(nlohmann::json& j, const package_t& package);

/*!
 * @brief SDK Interface
 *
 * An interface which describes the SDK and its configuration used to capture and transmit the event.
 */
class sdk_t
{
  public:
    /// the name of the SDK
    std::string name;

    /// the semantic version of the SDK
    std::string version;

    /// a list of integrations with the platform or a framework that were explicitly activated by the user
    std::vector<std::string> integrations;

    /// a list of packages that were installed as part of this SDK or the activated integrations
    std::vector<package_t> packages;
};

void to_json(nlohmann::json& j, const sdk_t& sdk);

} // namespace interfaces
} // namespace types
} // namespace crow
