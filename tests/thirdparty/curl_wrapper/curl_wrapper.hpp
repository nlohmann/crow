#pragma once

#include <thirdparty/json/json.hpp>

using json = nlohmann::json;
extern json results;

/// a counter how often POST has been called
size_t message_count = 0;

class curl_wrapper
{
  public:
    std::string post(const std::string& url, const json& payload, bool=false)
    {
        results.push_back({{"payload", payload}, {"url", url}});
        return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
    }

    std::string post(const std::string& url, const std::string &data, bool=false)
    {
        results.push_back({{"payload", data}, {"url", url}});
        return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
    }

    void set_header(const char*)
    {}
};
