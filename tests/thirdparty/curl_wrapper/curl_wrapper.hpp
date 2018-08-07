#pragma once

#include <thirdparty/json/json.hpp>

using json = nlohmann::json;

class curl_wrapper
{
  public:
    std::string post(const std::string& url, const json& payload, bool = false)
    {
        results.push_back({{"payload", payload}, {"url", url}});
        return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
    }

    std::string post(const std::string& url, const std::string& data, bool = false)
    {
        results.push_back({{"payload", data}, {"url", url}});
        return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
    }

    void set_header(const char*)
    {}

    static void reset()
    {
        message_count = 0;
        results.clear();
    }

    /// a counter how often POST has been called
    static size_t message_count;
    /// a list of sent messages
    static json results;
};
