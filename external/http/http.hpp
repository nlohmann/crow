/*               _
  ___ _   _ _ __| | __      ___ __ __ _ _ __  _ __   ___ _ __
 / __| | | | '__| | \ \ /\ / / '__/ _` | '_ \| '_ \ / _ \ '__|
| (__| |_| | |  | |  \ V  V /| | | (_| | |_) | |_) |  __/ |
 \___|\__,_|_|  |_|   \_/\_/ |_|  \__,_| .__/| .__/ \___|_|
                                       |_|   |_|                version 0.1.0
 */

#pragma once

#include <chrono>
#include <string>
#include <utility>
#include <zlib.h>
#include <curl/curl.h>
#include <json/json.hpp>

namespace http {

/*!
 * @brief wrapper around zlib to use RAII for resource management
 */
class gzip_wrapper
{
  public:
    /// create a gzip wrapper
    gzip_wrapper()
    {
        std::memset(&m_zs, 0, sizeof(m_zs));
        int ret = deflateInit2(&m_zs,
                               Z_BEST_COMPRESSION, // compression level
                               Z_DEFLATED,         // compression method
                               WINDOW_SIZE + 16,   // use 2**WINDOW_SIZE bytes memory and add gzip header (+16)
                               9,                  // optimize for speed and use more memory
                               Z_DEFAULT_STRATEGY  // compression strategy
        );

        if (ret != Z_OK) // LCOV_EXCL_LINE
        {
            // result was Z_MEM_ERROR, Z_STREAM_ERROR, or Z_VERSION_ERROR
            throw std::runtime_error("error during zlib initialization: (" + std::to_string(ret) + ") " + m_zs.msg); // LCOV_EXCL_LINE
        }
    }

    /// clean up structures
    ~gzip_wrapper()
    {
        deflateEnd(&m_zs);
    }

    gzip_wrapper(const gzip_wrapper&) = delete;
    gzip_wrapper(gzip_wrapper&&) = delete;
    gzip_wrapper& operator=(const gzip_wrapper&) = delete;
    gzip_wrapper& operator=(gzip_wrapper&&) = delete;

    /*!
     * @brief gzip compress a string
     *
     * @param[in] input_string string to compress
     * @return gzip-compressed string
     *
     * @throw std::runtime_error in case of errors
     *
     * @note Code from <https://panthema.net/2007/0328-ZLibString.html>. Adjusted by Niels Lohmann.
     *
     * @copyright Copyright 2007 Timo Bingmann <tb @ panthema.net>.
     *            Distributed under the Boost Software License, Version 1.0.
     *            (See http://www.boost.org/LICENSE_1_0.txt)
     */
    std::string compress(const std::string& input_string)
    {
        // connect input to zstream
        m_zs.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(input_string.data()));
        m_zs.avail_in = static_cast<uInt>(input_string.size());

        std::string result;
        int ret;

        // retrieve the compressed bytes blockwise
        do
        {
            m_zs.next_out = reinterpret_cast<Bytef*>(m_output_buffer.data());
            m_zs.avail_out = static_cast<uInt>(m_output_buffer.size());

            ret = deflate(&m_zs, Z_FINISH);

            if (result.size() < m_zs.total_out)
            {
                // append the block to the output string
                result.append(m_output_buffer.data(), m_zs.total_out - result.size());
            }
        } while (ret == Z_OK);

        if (ret != Z_STREAM_END) // LCOV_EXCL_LINE
        {
            // an error occurred that was not EOF (Z_STREAM_ERROR or Z_BUF_ERROR)
            throw std::runtime_error("error during zlib compression: (" + std::to_string(ret) + ") " + m_zs.msg); // LCOV_EXCL_LINE
        }

        return result;
    }

  private:
    /// zlib's main handle
    z_stream m_zs{};
    /// window size (must be synchronized with size of output buffer)
    static constexpr unsigned WINDOW_SIZE{15};
    /// output buffer; size is 2**WINDOW_SIZE; that is, 2**15 = 32768
    std::array<char, (1U << WINDOW_SIZE)> m_output_buffer{};
};

namespace status {

using code_t = unsigned int;

static constexpr code_t CONTINUE = 100;
static constexpr code_t SWITCHING_PROTOCOLS = 101;
static constexpr code_t OK = 200;
static constexpr code_t CREATED = 201;
static constexpr code_t ACCEPTED = 202;
static constexpr code_t NON_AUTHORITATIVE_INFORMATION = 203;
static constexpr code_t NO_CONTENT = 204;
static constexpr code_t RESET_CONTENT = 205;
static constexpr code_t PARTIAL_CONTENT = 206;
static constexpr code_t MULTIPLE_CHOICES = 300;
static constexpr code_t MOVED_PERMANENTLY = 301;
static constexpr code_t MOVED_TEMPORARILY = 302;
static constexpr code_t SEE_OTHER = 303;
static constexpr code_t NOT_MODIFIED = 304;
static constexpr code_t USE_PROXY = 305;
static constexpr code_t BAD_REQUEST = 400;
static constexpr code_t UNAUTHORIZED = 401;
static constexpr code_t PAYMENT_REQUIRED = 402;
static constexpr code_t FORBIDDEN = 403;
static constexpr code_t NOT_FOUND = 404;
static constexpr code_t METHOD_NOT_ALLOWED = 405;
static constexpr code_t NOT_ACCEPTABLE = 406;
static constexpr code_t PROXY_AUTHENTICATION_REQUIRED = 407;
static constexpr code_t REQUEST_TIME_OUT = 408;
static constexpr code_t CONFLICT = 409;
static constexpr code_t GONE = 410;
static constexpr code_t LENGTH_REQUIRED = 411;
static constexpr code_t PRECONDITION_FAILED = 412;
static constexpr code_t REQUEST_ENTITY_TOO_LARGE = 413;
static constexpr code_t REQUEST_URI_TOO_LARGE = 414;
static constexpr code_t UNSUPPORTED_MEDIA_TYPE = 415;
static constexpr code_t INTERNAL_SERVER_ERROR = 500;
static constexpr code_t NOT_IMPLEMENTED = 501;
static constexpr code_t BAD_GATEWAY = 502;
static constexpr code_t SERVICE_UNAVAILABLE = 503;
static constexpr code_t GATEWAY_TIME_OUT = 504;
static constexpr code_t HTTP_VERSION_NOT_SUPPORTED = 505;

/*!
 * @brief return name of a status code
 * @param status status code
 * @return name of @a status
 */
const char* name(code_t status) noexcept
{
    switch (status)
    {
        case CONTINUE:
            return "Continue";
        case SWITCHING_PROTOCOLS:
            return "Switching Protocols";
        case OK:
            return "OK";
        case CREATED:
            return "Created";
        case ACCEPTED:
            return "Accepted";
        case NON_AUTHORITATIVE_INFORMATION:
            return "Non-Authoritative Information";
        case NO_CONTENT:
            return "No Content";
        case RESET_CONTENT:
            return "Reset Content";
        case PARTIAL_CONTENT:
            return "Partial Content";
        case MULTIPLE_CHOICES:
            return "Multiple Choices";
        case MOVED_PERMANENTLY:
            return "Moved Permanently";
        case MOVED_TEMPORARILY:
            return "Moved Temporarily";
        case SEE_OTHER:
            return "See Other";
        case NOT_MODIFIED:
            return "Not Modified";
        case USE_PROXY:
            return "Use Proxy";
        case BAD_REQUEST:
            return "Bad Request";
        case UNAUTHORIZED:
            return "Unauthorized";
        case PAYMENT_REQUIRED:
            return "Payment Required";
        case FORBIDDEN:
            return "Forbidden";
        case NOT_FOUND:
            return "Not Found";
        case METHOD_NOT_ALLOWED:
            return "Method Not Allowed";
        case NOT_ACCEPTABLE:
            return "Not Acceptable";
        case PROXY_AUTHENTICATION_REQUIRED:
            return "Proxy Authentication Required";
        case REQUEST_TIME_OUT:
            return "Request Timeout";
        case CONFLICT:
            return "Conflict";
        case GONE:
            return "Gone";
        case LENGTH_REQUIRED:
            return "Length Required";
        case PRECONDITION_FAILED:
            return "Precondition Failed";
        case REQUEST_ENTITY_TOO_LARGE:
            return "Request Entity Too Large";
        case REQUEST_URI_TOO_LARGE:
            return "URI Too Long";
        case UNSUPPORTED_MEDIA_TYPE:
            return "Unsupported Media Type";
        case INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case NOT_IMPLEMENTED:
            return "Not Implemented";
        case BAD_GATEWAY:
            return "Bad Gateway";
        case SERVICE_UNAVAILABLE:
            return "Service Unavailable";
        case GATEWAY_TIME_OUT:
            return "Gateway Timeout";
        case HTTP_VERSION_NOT_SUPPORTED:
            return "HTTP Version Not Supported";
        default:
            return "unknown";
    }
}

} // namespace status

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
    exception(const char* what, const char* detailed_msg, CURLcode code)
        : std::runtime_error(what)
        , details(detailed_msg)
        , curl_code(code)
    {}

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
     */
    status_exception(const char* what,
                     status::code_t status,
                     const char* detailed_msg)
        : exception(what, detailed_msg, CURLE_OK)
        , status_code(status)
        , reason(status::name(status))
    {}

    /// integer code of the HTTP status
    const status::code_t status_code;
    /// textual reason of responded HTTP Status, e.g. "Not Found" or "OK"
    const char* reason;
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
     */
    explicit timeout_exception(const char* what, const char* detailed_msg, CURLcode code)
        : exception(what, detailed_msg, code)
    {}
};

/*!
 * @brief a response to a HTTP request
 */
class response
{
  public:
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
    nlohmann::json json() const
    {
        return nlohmann::json::parse(text);
    }

    /*!
     * @brief throw an exception if the HTTP status code is 4xx-5xx
     * @throw status_exception if status_code is 400..599
     * @note  invalid status codes (e.g., 600) do not throw; this is the way requests is doing it, see
     *        <https://2.python-requests.org/en/master/_modules/requests/models/#Response.raise_for_status>
     */
    void throw_for_status() const
    {
        if (400 <= status_code and status_code < 500)
        {
            throw status_exception("client error", status_code, "");
        }
        if (500 <= status_code and status_code < 600)
        {
            throw status_exception("server error", status_code, "");
        }
    }
};

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
    request() noexcept
        : m_curl(curl_easy_init())
    {
        assert(m_curl);
        set_default_values();
    }

    /*!
     * @brief clean up
     */
    ~request() noexcept
    {
        clear_headers();
        curl_easy_cleanup(m_curl);
    }

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
    http::response get(const std::string& url)
    {
        set_option(CURLOPT_URL, url.c_str());
        set_option(CURLOPT_WRITEFUNCTION, &write_callback);
        set_option(CURLOPT_WRITEDATA, &m_string_buffer);
        set_option(CURLOPT_ACCEPT_ENCODING, "");

        m_error_buffer[0] = 0;
        m_string_buffer.clear();

        const auto res = curl_easy_perform(m_curl);
        check_result(res);

        return create_response();
    }

    /*!
     * @brief POST request with JSON payload
     * @param[in] url URL to query
     * @param[in] payload JSON payload
     * @return request result
     */
    http::response post(const std::string& url,
                        const nlohmann::json& payload)
    {
        set_header("Content-Type: application/json");
        return post(url, payload.dump());
    }

    /*!
     * @brief POST request
     * @param[in] url URL to query
     * @param[in] data payload
     * @return request result
     */
    http::response post(const std::string& url,
                        const std::string& data)
    {
        // the string must live as long as the curl command does
        std::string compressed_data;

        if (m_use_compression)
        {
            compressed_data = gzip_wrapper().compress(data);

            set_header("Content-Encoding: gzip");
            std::string size_header = "Content-Length: " + std::to_string(compressed_data.size());
            set_header(size_header.c_str());
            set_option(CURLOPT_POSTFIELDS, compressed_data.c_str());
            set_option(CURLOPT_POSTFIELDSIZE, compressed_data.size());
        }
        else
        {
            set_option(CURLOPT_POSTFIELDS, data.c_str());
            set_option(CURLOPT_POSTFIELDSIZE, data.size());
        }

        set_option(CURLOPT_URL, url.c_str());
        set_option(CURLOPT_POST, 1L);
        set_option(CURLOPT_WRITEFUNCTION, &write_callback);
        set_option(CURLOPT_WRITEDATA, &m_string_buffer);

        m_error_buffer[0] = 0;
        m_string_buffer.clear();

        const auto res = curl_easy_perform(m_curl);
        check_result(res);

        // reset values
        set_option(CURLOPT_POST, 0L);

        return create_response();
    }

    ///@}

    ///@{
    /// @name options

    /*!
     * @brief resets request to default state
     */
    void reset() noexcept
    {
        curl_easy_reset(m_curl);
        clear_headers();
        set_default_values();
    }

    /*!
     * @brief set a timeout for the request
     * @param[in] value value to set (default: no timeout)
     * @return configured curl_wrapper
     * @sa https://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT_MS.html
     */
    request& timeout(std::chrono::milliseconds value = std::chrono::milliseconds(0)) noexcept
    {
        set_option(CURLOPT_TIMEOUT_MS, value.count());
        return *this;
    }

    /*!
     * @brief whether to follow redirects
     * @param[in] value value to set (default: true)
     * @return configured curl_wrapper
     * @sa https://curl.haxx.se/libcurl/c/CURLOPT_FOLLOWLOCATION.html
     */
    request& follow_redirects(bool value = true) noexcept
    {
        set_option(CURLOPT_FOLLOWLOCATION, value ? 1L : 0L);
        return *this;
    }

    /*!
     * @brief whether to use gzip compression for POST payloads
     * @param[in] value value to set (default: false)
     * @return configured curl_wrapper
     */
    request& use_compression(bool value = false) noexcept
    {
        m_use_compression = value;
        return *this;
    }

    /*!
     * @brief whether to print verbose information to stderr
     * @param[in] value value to set (default: false)
     * @return configured curl_wrapper
     * @sa https://curl.haxx.se/libcurl/c/CURLOPT_VERBOSE.html
     */
    request& be_verbose(bool value = false) noexcept
    {
        set_option(CURLOPT_VERBOSE, value ? 1L : 0L);
        return *this;
    }

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
    void set_header(const char* header) noexcept
    {
        m_headers = curl_slist_append(m_headers, header);
        set_option(CURLOPT_HTTPHEADER, m_headers);
    }

    /*!
     * @brief reset headers
     */
    void clear_headers() noexcept
    {
        curl_slist_free_all(m_headers);
    }

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
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
    {
        assert(userdata);
        const auto bytes = size * nmemb;
        static_cast<std::string*>(userdata)->append(ptr, bytes);
        return bytes;
    }

    std::int64_t get_info_long(CURLINFO option) noexcept
    {
        std::int64_t result;
        curl_easy_getinfo(m_curl, option, &result); // NOLINT(hicpp-vararg)
        return result;
    }

    double get_info_double(CURLINFO option) noexcept
    {
        double result;
        curl_easy_getinfo(m_curl, option, &result); // NOLINT(hicpp-vararg)
        return result;
    }

    const char* get_info_string(CURLINFO option) noexcept
    {
        char* result;
        curl_easy_getinfo(m_curl, option, &result); // NOLINT(hicpp-vararg)
        return result;
    }

    void set_default_values() noexcept
    {
        m_error_buffer[0] = 0;
        set_option(CURLOPT_ERRORBUFFER, m_error_buffer.data());
        set_option(CURLOPT_SSL_VERIFYPEER, 0L);
        timeout();
        follow_redirects();
        use_compression();
        be_verbose();
    }

    http::response create_response() noexcept
    {
        const auto status_code = static_cast<status::code_t>(get_info_long(CURLINFO_RESPONSE_CODE));
        const auto duration_milliseconds = static_cast<std::int64_t>(std::micro::den * get_info_double(CURLINFO_STARTTRANSFER_TIME));

        return {
            // status code
            status_code,
            // reason
            http::status::name(status_code),
            // ok
            status_code < 400,
            // text
            std::move(m_string_buffer),
            // url
            get_info_string(CURLINFO_EFFECTIVE_URL),
            // elapsed
            std::chrono::microseconds(duration_milliseconds)};
    }

    void check_result(CURLcode result) const
    {
        switch (result)
        {
            case CURLE_OK:
                return;

            case CURLE_OPERATION_TIMEDOUT:
                throw http::timeout_exception(curl_easy_strerror(result), m_error_buffer.data(), result);

            default:
                throw http::exception(curl_easy_strerror(result), m_error_buffer.data(), result);
        }
    }

  private:
    /// curl handle
    CURL* const m_curl;
    /// list for HTTP headers
    struct curl_slist* m_headers{nullptr};
    /// whether to use gzip compression for POST payloads
    bool m_use_compression{false};
    /// a buffer to store results
    std::string m_string_buffer{};
    /// buffer for error messages
    std::array<char, CURL_ERROR_SIZE> m_error_buffer{};
};

} // namespace http
