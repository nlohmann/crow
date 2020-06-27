#pragma once

#include <list>
#include <string>
#include <crow/types/helpers/base_types.hpp>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

enum class breadcrumb_type_t
{
    default_,  ///< no specific handling
    http,      ///< HTTP requests
    navigation ///< navigation events
};

void to_json(nlohmann::json& j, const breadcrumb_type_t& breadcrumb_type);

class navigation_breadcrumb_data_t
{
  public:
    /// a string representing the original application state / location
    std::string from;

    /// a string representing the new application state / location
    std::string to;
};

void to_json(nlohmann::json& j, const navigation_breadcrumb_data_t& navigation_breadcrumb_data);

class http_breadcrumb_data_t
{
  public:
    /// the request URL
    std::string url;

    /// the HTTP request method
    std::string method;

    /// the HTTP status code of the response
    int status_code{-1};

    /// a text that describes the status code
    std::string reason;
};

void to_json(nlohmann::json& j, const http_breadcrumb_data_t& http_breadcrumb_data);

class breadcrumb_value_t
{
  public:
    /// a timestamp representing when the breadcrumb occurred
    std::string timestamp;

    /// a timestamp representing when the breadcrumb occurred
    long timestamp_unix{-1};

    /// the type of breadcrumb
    breadcrumb_type_t type{breadcrumb_type_t::default_};

    /// a dotted string indicating what the crumb is or where it comes from
    std::string category{};

    /// message associated to the breadcrumb
    std::string message{};

    /// key/value table for default breadcrumbs
    nlohmann::json data{};

    /// this defines the severity level of the breadcrumb
    level_t level{level_t::info};

    /// navigation breadcrumb (used if m_type == navigation)
    navigation_breadcrumb_data_t navigation_data{};

    /// HTTP request breadcrumb (used if m_type == http)
    http_breadcrumb_data_t request_data{};
};

void to_json(nlohmann::json& j, const breadcrumb_value_t& breadcrumb_value);

class breadcrumbs_t
{
  public:
    std::list<interfaces::breadcrumb_value_t> values;
};

void to_json(nlohmann::json& j, const breadcrumbs_t& breadcrumbs);

} // namespace interfaces
} // namespace types
} // namespace crow
