#pragma once

#include <array>
#include <string>
#include <zlib.h>

namespace http {

/*!
 * @brief wrapper around zlib to use RAII for resource management
 */
class gzip_wrapper
{
  public:
    /// create a gzip wrapper
    gzip_wrapper();

    /// clean up structures
    ~gzip_wrapper() noexcept;

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
    std::string compress(const std::string& input_string);

  private:
    /// zlib's main handle
    z_stream m_zs{};
    /// window size (must be synchronized with size of output buffer)
    static constexpr unsigned WINDOW_SIZE{15};
    /// output buffer; size is 2**WINDOW_SIZE; that is, 2**15 = 32768
    std::array<char, (1U << WINDOW_SIZE)> m_output_buffer{};
};

} // namespace http
