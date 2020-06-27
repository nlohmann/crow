#include <algorithm>
#include <http/detail/exception.hpp>
#include <http/detail/gzip_wrapper.hpp>
#include <http/detail/request.hpp>

namespace http {

curl_initializer initializer;

request::request() noexcept
    : m_curl(curl_easy_init())
{
    assert(m_curl);
    set_default_values();
}

request::~request() noexcept
{
    clear_headers();
    curl_easy_cleanup(m_curl);
}

http::response request::get(const std::string& url)
{
    std::string buffer;

    set_option(CURLOPT_URL, url.c_str());
    set_option(CURLOPT_WRITEFUNCTION, &write_callback);
    set_option(CURLOPT_WRITEDATA, &buffer);
    set_option(CURLOPT_ACCEPT_ENCODING, "");

    m_error_buffer[0] = 0;

    const auto res = curl_easy_perform(m_curl);
    check_result(res);

    return create_response(std::move(buffer));
}

http::response request::post(const std::string& url,
                             const nlohmann::json& payload)
{
    set_header("Content-Type: application/json");
    return post(url, payload.dump());
}

http::response request::post(const std::string& url,
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

    std::string buffer;

    set_option(CURLOPT_URL, url.c_str());
    set_option(CURLOPT_POST, 1L);
    set_option(CURLOPT_WRITEFUNCTION, &write_callback);
    set_option(CURLOPT_WRITEDATA, &buffer);

    m_error_buffer[0] = 0;

    const auto res = curl_easy_perform(m_curl);
    check_result(res);

    // reset values
    set_option(CURLOPT_POST, 0L);

    return create_response(std::move(buffer));
}

void request::reset() noexcept
{
    curl_easy_reset(m_curl);
    clear_headers();
    set_default_values();
}

request& request::timeout(std::chrono::milliseconds value) noexcept
{
    set_option(CURLOPT_TIMEOUT_MS, value.count());
    return *this;
}

request& request::follow_redirects(bool value) noexcept
{
    set_option(CURLOPT_FOLLOWLOCATION, value ? 1L : 0L);
    return *this;
}

request& request::use_compression(bool value) noexcept
{
    m_use_compression = value;
    return *this;
}

request& request::be_verbose(bool value) noexcept
{
    set_option(CURLOPT_VERBOSE, value ? 1L : 0L);
    return *this;
}

void request::set_header(const char* header) noexcept
{
    m_headers = curl_slist_append(m_headers, header);
    set_option(CURLOPT_HTTPHEADER, m_headers);
}

void request::clear_headers() noexcept
{
    curl_slist_free_all(m_headers);
}

size_t request::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    assert(userdata);
    const auto bytes = size * nmemb;
    static_cast<std::string*>(userdata)->append(ptr, bytes);
    return bytes;
}

std::int64_t request::get_info_long(CURLINFO option) noexcept
{
    std::int64_t result;
    curl_easy_getinfo(m_curl, option, &result); // NOLINT(hicpp-vararg)
    return result;
}

double request::get_info_double(CURLINFO option) noexcept
{
    double result;
    curl_easy_getinfo(m_curl, option, &result); // NOLINT(hicpp-vararg)
    return result;
}

const char* request::get_info_string(CURLINFO option) noexcept
{
    char* result;
    curl_easy_getinfo(m_curl, option, &result); // NOLINT(hicpp-vararg)
    return result;
}

void request::set_default_values() noexcept
{
    m_error_buffer[0] = 0;
    set_option(CURLOPT_ERRORBUFFER, m_error_buffer.data());
    set_option(CURLOPT_SSL_VERIFYPEER, 0L);
    timeout();
    follow_redirects();
    use_compression();
    be_verbose();
}

http::response request::create_response(std::string&& buffer) noexcept
{
    const auto status_code = static_cast<status::code_t>(get_info_long(CURLINFO_RESPONSE_CODE));
    const auto duration_milliseconds = static_cast<std::int64_t>(std::micro::den * get_info_double(CURLINFO_STARTTRANSFER_TIME));
    return response::create(status_code, std::move(buffer), get_info_string(CURLINFO_EFFECTIVE_URL), std::chrono::microseconds(duration_milliseconds));
}

void request::check_result(CURLcode result) const
{
    switch (result)
    {
        case CURLE_OK:
            return;

        case CURLE_OPERATION_TIMEDOUT:
            throw http::timeout_exception(curl_easy_strerror(result), m_error_buffer.data(), result, nullptr);

        default:
            throw http::exception(curl_easy_strerror(result), m_error_buffer.data(), result);
    }
}

} // namespace http
