#include <crow/types/interfaces/breadcrumbs.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const breadcrumb_type_t& breadcrumb_type)
{
    switch (breadcrumb_type)
    {
        case breadcrumb_type_t::default_:
            j = "default";
            break;
        case breadcrumb_type_t::http:
            j = "http";
            break;
        case breadcrumb_type_t::navigation:
            j = "navigation";
            break;
    }
}

void to_json(nlohmann::json& j, const navigation_breadcrumb_data_t& navigation_breadcrumb_data)
{
    fill_if_not_empty("from", navigation_breadcrumb_data.from, j);
    fill_if_not_empty("to", navigation_breadcrumb_data.to, j);
}

void to_json(nlohmann::json& j, const http_breadcrumb_data_t& http_breadcrumb_data)
{
    fill_if_not_empty("url", http_breadcrumb_data.url, j);
    fill_if_not_empty("method", http_breadcrumb_data.method, j);
    fill_if_not_empty("status_code", http_breadcrumb_data.status_code, j);
    fill_if_not_empty("reason", http_breadcrumb_data.reason, j);
}

void to_json(nlohmann::json& j, const breadcrumb_value_t& breadcrumb_value)
{
    if (not breadcrumb_value.timestamp.empty())
    {
        j["timestamp"] = breadcrumb_value.timestamp;
    }
    else if (breadcrumb_value.timestamp_unix > 0)
    {
        j["timestamp"] = breadcrumb_value.timestamp_unix;
    }

    fill_if_not_empty("type", nlohmann::json(breadcrumb_value.type), j);
    fill_if_not_empty("category", breadcrumb_value.category, j);
    fill_if_not_empty("message", breadcrumb_value.message, j);
    fill_if_not_empty("level", nlohmann::json(breadcrumb_value.level), j);

    switch (breadcrumb_value.type)
    {
        case breadcrumb_type_t::default_:
        {
            fill_if_not_empty("data", breadcrumb_value.data, j);
            break;
        }

        case breadcrumb_type_t::http:
        {
            fill_if_not_empty("data", nlohmann::json(breadcrumb_value.request_data), j);
            if (breadcrumb_value.data.is_object())
            {
                j["data"].update(breadcrumb_value.data);
            }
            break;
        }

        case breadcrumb_type_t::navigation:
        {
            fill_if_not_empty("data", nlohmann::json(breadcrumb_value.navigation_data), j);
            if (breadcrumb_value.data.is_object())
            {
                j["data"].update(breadcrumb_value.data);
            }
            break;
        }
    }
}

void to_json(nlohmann::json& j, const breadcrumbs_t& breadcrumbs)
{
    fill_if_not_empty("values", breadcrumbs.values, j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
