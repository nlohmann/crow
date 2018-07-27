#pragma once

#include <thirdparty/json/json.hpp>

using json = nlohmann::json;
extern json results;

class curl_wrapper
{
  public:
    std::string post(const std::string& url, const json& payload)
    {
        results.push_back({{"payload", payload}, {"url", url}});
        return "";
    }

    std::string post(const std::string& url, const std::string &data)
    {
        results.push_back({{"payload", data}, {"url", url}});
        return "";
    }

    void set_header(const char* header)
    {}
};
