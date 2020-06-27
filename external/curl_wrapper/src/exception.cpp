#include <http/detail/exception.hpp>

namespace http {

exception::exception(const char* what,
                     const char* detailed_msg,
                     CURLcode code) noexcept
    : std::runtime_error(what)
    , details(detailed_msg)
    , curl_code(code)
{}

status_exception::status_exception(const char* what,
                                   status::code_t status,
                                   const char* detailed_msg,
                                   const http::response* resp) noexcept
    : exception(what, detailed_msg, CURLE_OK)
    , status_code(status)
    , reason(status::name(status))
    , response(resp)
{}

timeout_exception::timeout_exception(const char* what,
                                     const char* detailed_msg,
                                     CURLcode code,
                                     const http::response* resp) noexcept
    : exception(what, detailed_msg, code)
    , response(resp)
{}

} // namespace http
