#pragma once

#include <string>
#include <iostream>
#include <curl/curl.h>
#include "thirdparty/json/json.hpp"

using json = nlohmann::json;

class curl_wrapper
{
  public:
    curl_wrapper() : m_curl(curl_easy_init()) {
        assert(m_curl);

        //set_option(CURLOPT_VERBOSE, 1L);
        set_option(CURLOPT_SSL_VERIFYPEER, 0L);
    }

    ~curl_wrapper() {
        curl_easy_cleanup(m_curl);
        curl_global_cleanup();
    }

    std::string post(const std::string& url, const json& payload)
    {
        set_header("Content-Type: application/json");
        return post(url, payload.dump());
    }

    std::string post(const std::string& url, const std::string &data)
    {
        set_option(CURLOPT_URL, url.c_str());
        set_option(CURLOPT_POSTFIELDS, data.c_str());
        set_option(CURLOPT_POSTFIELDSIZE, data.size());
        set_option(CURLOPT_POST, 1);
        set_option(CURLOPT_WRITEFUNCTION, &write_callback);
        set_option(CURLOPT_WRITEDATA, &string_buffer);

        auto res = curl_easy_perform(m_curl);

        if(res != CURLE_OK)
        {
            std::string error_msg = std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res);
            throw std::runtime_error(error_msg);
        }

        return string_buffer;
    }

    template<typename T>
    CURLcode set_option(CURLoption option, T parameter)
    {
        return curl_easy_setopt(m_curl, option, parameter);
    }

    void set_header(const char* header)
    {
        m_headers = curl_slist_append(m_headers, header);
        set_option(CURLOPT_HTTPHEADER, m_headers);
    }

  private:
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        assert(userdata);
        ((std::string*)userdata)->append(ptr, size * nmemb);
        return size * nmemb;
    }

  private:
    CURL * const m_curl;
    struct curl_slist *m_headers = nullptr;
    std::string string_buffer;
};
