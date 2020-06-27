#pragma once

namespace http {
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
extern const char* name(code_t status) noexcept;

} // namespace status
} // namespace http
