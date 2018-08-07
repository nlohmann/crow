#pragma once

#include <thirdparty/json/json.hpp>
#include <mutex>

using json = nlohmann::json;

class curl_wrapper
{
  public:
    std::string post(const std::string& url, const json& payload, bool = false)
    {
        std::lock_guard<std::mutex> lock(m_results_mutex);
        m_results.push_back({{"payload", payload}, {"url", url}});
        return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
    }

    std::string post(const std::string& url, const std::string& data, bool = false)
    {
        std::lock_guard<std::mutex> lock(m_results_mutex);
        m_results.push_back({{"payload", data}, {"url", url}});
        return "{\"id\":\"" + std::to_string(message_count++) + "\"}";
    }

    void set_header(const char*)
    {}

    static void reset()
    {
        message_count = 0;
        std::lock_guard<std::mutex> lock(m_results_mutex);
        m_results.clear();
    }

    static const json& results()
    {
        return m_results;
    }

  private:
    /// a list of sent messages
    static json m_results;

    /// a counter how often POST has been called
    static size_t message_count;

    /// a mutex to guard the results
    static std::mutex m_results_mutex;
};
