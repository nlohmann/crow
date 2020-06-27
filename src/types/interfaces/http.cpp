#include <crow/types/interfaces/http.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const http_t& http)
{
    fill_if_not_empty("url", http.url, j);
    fill_if_not_empty("method", http.method, j);
    fill_if_not_empty("data", http.data, j);
    fill_if_not_empty("query_string", http.query_string, j);
    fill_if_not_empty("cookies", http.cookies, j);
    fill_if_not_empty("headers", http.headers, j);
    fill_if_not_empty("env", http.env, j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
