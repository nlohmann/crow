#pragma once

#include <string>
#include <unordered_map>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

/*!
 * @brief HTTP Interface
 *
 * The Request information is stored in the HTTP interface.
 *
 * @sa https://docs.sentry.io/development/sdk-dev/interfaces/#http-interface
 */
class http_t
{
  public:
    /// the full URL of the request if available
    std::string url;

    /// the actual HTTP method of the request
    std::string method;

    /// submitted data in whatever format makes most sense
    nlohmann::json data;

    /// the unparsed query string as it is provided
    std::string query_string;

    /// the cookie values
    std::string cookies;

    /// a dictionary of submitted headers
    std::unordered_map<std::string, std::string> headers;

    /// optional environment data
    std::unordered_map<std::string, std::string> env;
};

void to_json(nlohmann::json& j, const http_t& http);

} // namespace interfaces
} // namespace types
} // namespace crow
