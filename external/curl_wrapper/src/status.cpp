#include <http/detail/status.hpp>

namespace http {
namespace status {

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
} // namespace http
