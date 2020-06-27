#pragma once

#include "status.hpp"
#include <chrono>
#include <string>
#include <nlohmann/json.hpp>

namespace http {

/*!
 * @brief a response to a HTTP request
 */
class response
{
  public:
    /// convenience constructor
    static response create(status::code_t status_code, std::string text, std::string url, std::chrono::microseconds elapsed);

    /// integer code of the HTTP status
    status::code_t status_code;

    /// textual reason of responded HTTP Status, e.g. "Not Found" or "OK"
    std::string reason;

    /// true if status code is less than 400
    bool ok;

    /// content of the response
    std::string text;

    /// final URL location
    std::string url;

    /// time until the first byte was received
    std::chrono::microseconds elapsed;

    /// JSON encoded response
    nlohmann::json json() const;

    /*!
     * @brief throw an exception if the HTTP status code is 4xx-5xx
     * @throw status_exception if status_code is 400..599
     * @note  invalid status codes (e.g., 600) do not throw; this is the way requests is doing it, see
     *        <https://2.python-requests.org/en/master/_modules/requests/models/#Response.raise_for_status>
     */
    void throw_for_status() const;
};

} // namespace http
