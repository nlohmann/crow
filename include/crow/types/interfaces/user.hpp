#pragma once

#include <string>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

/*!
 * @brief User Interface
 *
 * An interface which describes the authenticated User for a request.
 *
 * You should provide at least either an id (a unique identifier for an authenticated user) or ip_address (their IP address).
 */
class user_t
{
  public:
    /// the unique ID of the user
    std::string id;

    /// the email address of the user
    std::string email;

    /// the IP of the user
    std::string ip_address;

    /// the username of the user
    std::string username;

    /// additional information
    nlohmann::json extra;
};

void to_json(nlohmann::json& j, const user_t& user);

} // namespace interfaces
} // namespace types
} // namespace crow
