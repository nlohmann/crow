#pragma once

#include <thirdparty/json/json.hpp>
#include <mutex>

using json = nlohmann::json;

class curl_wrapper
{
  public:
    std::string post(const std::string& url, const json& payload, bool = false);
    std::string post(const std::string& url, const std::string& data, bool = false);

    void set_header(const char*);

    static json results;
    static size_t message_count;
    static std::mutex mutex;
};
