#pragma once

namespace http {

/*!
 * @brief RAII wrapper fro thread-safe libcurl initialization
 * @sa https://curl.haxx.se/libcurl/c/curl_global_init.html
 * @sa https://curl.haxx.se/libcurl/c/curl_global_cleanup.html
 */
class curl_initializer
{
  public:
    /// call curl_global_init exactly once
    curl_initializer() noexcept;
    /// call curl_global_cleanup exactly once
    ~curl_initializer() noexcept;

    curl_initializer(const curl_initializer&) = delete;
    curl_initializer(curl_initializer&&) = delete;
    curl_initializer& operator=(const curl_initializer&) = delete;
    curl_initializer& operator=(curl_initializer&&) = delete;
};

} // namespace http
