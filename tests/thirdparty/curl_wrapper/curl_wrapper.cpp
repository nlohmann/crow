#include <thirdparty/curl_wrapper/curl_wrapper.hpp>

json curl_wrapper::results = json::array();
size_t curl_wrapper::message_count = 0;
std::mutex curl_wrapper::mutex;

std::string curl_wrapper::post(const std::string& url, const json& payload, bool)
{
    std::lock_guard<std::mutex> lock(mutex);
    results.push_back({{"payload", payload}, {"url", url}});
    return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
}

std::string curl_wrapper::post(const std::string& url, const std::string& data, bool)
{
    std::lock_guard<std::mutex> lock(mutex);
    results.push_back({{"payload", data}, {"url", url}});
    return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
}

void curl_wrapper::set_header(const char*)
{}
