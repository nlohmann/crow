#include <curl/curl.h>
#include <http/detail/curl_initializer.hpp>

namespace http {

curl_initializer::curl_initializer() noexcept
{
    curl_global_init(CURL_GLOBAL_ALL); // NOLINT(hicpp-signed-bitwise)
}

curl_initializer::~curl_initializer() noexcept
{
    curl_global_cleanup();
}

} // namespace http
