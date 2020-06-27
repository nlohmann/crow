#include <algorithm>
#include <http/detail/exception.hpp>
#include <http/detail/response.hpp>

namespace http {

response response::create(status::code_t status_code, std::string text, std::string url, std::chrono::microseconds elapsed)
{
    return {status_code, status::name(status_code), status_code < 400, std::move(text), std::move(url), elapsed};
}

nlohmann::json response::json() const
{
    return nlohmann::json::parse(text);
}

void response::throw_for_status() const
{
    if (400 <= status_code and status_code < 500)
    {
        throw status_exception("client error", status_code, "", this);
    }
    if (500 <= status_code and status_code < 600)
    {
        throw status_exception("server error", status_code, "", this);
    }
}

} // namespace http
