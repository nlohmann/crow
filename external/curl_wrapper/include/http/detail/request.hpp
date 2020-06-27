#pragma once

#include "curl_initializer.hpp"
#include "response.hpp"
#include "status.hpp"
#include <array>
#include <chrono>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace http {

/*!
 * @brief a HTTP request
 */
class request
{
  public:
    ///@{
    /// @name constructor/destructor

    /*!
     * @brief create a wrapper object
     */
    request() noexcept;

    /*!
     * @brief clean up
     */
    ~request() noexcept;

    // remove special functions due to pointer members
    request(const request&) = delete;
    request(request&&) = delete;
    request& operator=(const request&) = delete;
    request& operator=(request&&) = delete;

    ///@}

    ///@{
    /// @name operations

    /*!
     * @brief GET request
     * @param[in] url URL to query
     * @return request result
     */
    response get(const std::string& url);

    /*!
     * @brief POST request with JSON payload
     * @param[in] url URL to query
     * @param[in] payload JSON payload
     * @return request result
     */
    response post(const std::string& url,
                  const nlohmann::json& payload);

    /*!
     * @brief POST request
     * @param[in] url URL to query
     * @param[in] data payload
     * @return request result
     */
    response post(const std::string& url,
                  const std::string& data);

    ///@}

    ///@{
    /// @name options

    /*!
     * @brief resets request to default state
     */
    void reset() noexcept;

    /*!
     * @brief set a timeout for the request
     * @param[in] value value to set (default: no timeout)
     * @return configured curl_wrapper
     * @sa https://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT_MS.html
     */
    request& timeout(std::chrono::milliseconds value = std::chrono::milliseconds(0)) noexcept;

    /*!
     * @brief whether to follow redirects
     * @param[in] value value to set (default: true)
     * @return configured curl_wrapper
     * @sa https://curl.haxx.se/libcurl/c/CURLOPT_FOLLOWLOCATION.html
     */
    request& follow_redirects(bool value = true) noexcept;

    /*!
     * @brief whether to use gzip compression for POST payloads
     * @param[in] value value to set (default: false)
     * @return configured curl_wrapper
     */
    request& use_compression(bool value = false) noexcept;

    /*!
     * @brief whether to print verbose information to stderr
     * @param[in] value value to set (default: false)
     * @return configured curl_wrapper
     * @sa https://curl.haxx.se/libcurl/c/CURLOPT_VERBOSE.html
     */
    request& be_verbose(bool value = false) noexcept;

    /*!
     * @brief set libcurl option
     * @tparam CurlOptionType the different types supported by curl_easy_setopt
     * @param[in] option identifier of the option
     * @param[in] parameter value for the option
     * @return status code whether setting the option worked
     */
    template <typename CurlOptionType>
    CURLcode set_option(CURLoption option, CurlOptionType parameter) noexcept
    {
        return curl_easy_setopt(m_curl, option, parameter); // NOLINT(hicpp-vararg)
    }

    /*!
     * @brief set a header for subsequent requests
     * @param[in] header the header string to set, e.g. "Accept: application/json"
     */
    void set_header(const char* header) noexcept;

    /*!
     * @brief reset headers
     */
    void clear_headers() noexcept;

    ///@}

  private:
    /*!
     * @brief callback function for libcurl to store received data
     *
     * @param[in] ptr pointer to the received data
     * @param[in] size size of each data element (always 1)
     * @param[in] nmemb number of received data elements
     * @param[in] userdata pointer to user data to store the data to (set to @a m_string_buffer)
     * @return the number of bytes taken care of
     *
     * @sa https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html
     */
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata);

    std::int64_t get_info_long(CURLINFO option) noexcept;
    double get_info_double(CURLINFO option) noexcept;
    const char* get_info_string(CURLINFO option) noexcept;

    void set_default_values() noexcept;

    response create_response(std::string&& buffer) noexcept;

    void check_result(CURLcode result) const;

    /// a RAII wrapper to properly initialize libcurl
    static curl_initializer initializer;

  private:
    /// curl handle
    CURL* const m_curl;
    /// list for HTTP headers
    struct curl_slist* m_headers{nullptr};
    /// whether to use gzip compression for POST payloads
    bool m_use_compression{false};
    /// buffer for error messages
    std::array<char, CURL_ERROR_SIZE> m_error_buffer{};
};

} // namespace http
