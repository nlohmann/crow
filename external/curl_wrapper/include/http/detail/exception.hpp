#pragma once

#include "status.hpp"
#include <stdexcept>
#include <curl/curl.h>

namespace http {

class response;

/*!
 * @brief exception base class
 */
class exception : public std::runtime_error
{
  public:
    /*!
     * @brief create exception
     * @param[in] what explanatory string
     * @param[in] detailed_msg further details
     * @param[in] code return value of previous libcurl call
     */
    exception(const char* what, const char* detailed_msg, CURLcode code) noexcept;

    /// details on the exception
    const char* details;

    /// the return code of libcurl
    CURLcode curl_code;
};

/*!
 * @brief exception indicating a 4xx-5xx status
 */
class status_exception : public exception
{
  public:
    /*!
     * @brief create status code exception
     * @param[in] what explanatory string
     * @param[in] detailed_msg further details
     * @param[in] status status code
     * @param[in] resp associated response
     */
    status_exception(const char* what,
                     status::code_t status,
                     const char* detailed_msg,
                     const response* resp) noexcept;

    /// integer code of the HTTP status
    const status::code_t status_code;
    /// textual reason of responded HTTP Status, e.g. "Not Found" or "OK"
    const char* reason;
    /// associated response
    const http::response* response = nullptr;
};

/*!
 * @brief exception indicating a timeout
 */
class timeout_exception : public exception
{
  public:
    /*!
     * @brief create timeout exception
     * @param[in] what explanatory string
     * @param[in] detailed_msg further details
     * @param[in] code return value of previous libcurl call
     * @param[in] resp associated response
     */
    timeout_exception(const char* what,
                      const char* detailed_msg,
                      CURLcode code,
                      const response* resp) noexcept;

    /// associated response
    const http::response* response = nullptr;
};

} // namespace http
