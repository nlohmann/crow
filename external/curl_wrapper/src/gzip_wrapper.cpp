#include <cstring>
#include <stdexcept>
#include <http/detail/gzip_wrapper.hpp>

namespace http {

gzip_wrapper::gzip_wrapper()
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

gzip_wrapper::~gzip_wrapper() noexcept
{
    deflateEnd(&m_zs);
}

std::string gzip_wrapper::compress(const std::string& input_string)
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

} // namespace http
