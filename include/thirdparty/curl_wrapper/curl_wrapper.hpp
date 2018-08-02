#pragma once

#include <cstring>
#include <string>
#include <curl/curl.h>
#include <zlib.h>
#include "thirdparty/json/json.hpp"

using json = nlohmann::json;

class curl_wrapper
{
  public:
    curl_wrapper() : m_curl(curl_easy_init())
    {
        assert(m_curl);

        //set_option(CURLOPT_VERBOSE, 1L);
        set_option(CURLOPT_SSL_VERIFYPEER, 0L);
    }

    ~curl_wrapper()
    {
        curl_slist_free_all(m_headers);
        curl_easy_cleanup(m_curl);
        curl_global_cleanup();
    }

    std::string post(const std::string& url, const json& payload, const bool compress = false)
    {
        set_header("Content-Type: application/json");
        return post(url, payload.dump(), compress);
    }

    std::string post(const std::string& url, const std::string& data, const bool compress = false)
    {
        std::string c_data;

        if (compress)
        {
            c_data = compress_string(data);

            set_header("Content-Encoding: gzip");
            std::string size_header = "Content-Length: " + std::to_string(c_data.size());
            set_header(size_header.c_str());
            set_option(CURLOPT_POSTFIELDS, c_data.c_str());
            set_option(CURLOPT_POSTFIELDSIZE, c_data.size());
        }
        else
        {
            set_option(CURLOPT_POSTFIELDS, data.c_str());
            set_option(CURLOPT_POSTFIELDSIZE, data.size());
        }

        set_option(CURLOPT_URL, url.c_str());
        set_option(CURLOPT_POST, 1);
        set_option(CURLOPT_WRITEFUNCTION, &write_callback);
        set_option(CURLOPT_WRITEDATA, &string_buffer);

        auto res = curl_easy_perform(m_curl);

        if (res != CURLE_OK)
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
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
    {
        assert(userdata);
        ((std::string*)userdata)->append(ptr, size * nmemb);
        return size * nmemb;
    }

    /*!
     * @brief gzip compress a string
     *
     * @param[in] str string to compress
     * @return gzip-compressed string
     *
     * @throw std::runtime_error in case of errors
     *
     * @note Code from <https://panthema.net/2007/0328-ZLibString.html>. Adjusted by Niels Lohmann.
     *
     * @copyright Copyright 2007 Timo Bingmann <tb@panthema.net>.
     *            Distributed under the Boost Software License, Version 1.0.
     *            (See http://www.boost.org/LICENSE_1_0.txt)
     */
    std::string compress_string(const std::string& str)
    {
        std::string result;
        z_stream zs;                        // z_stream is zlib's control structure
        std::memset(&zs, 0, sizeof(zs));

        int ret = deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 9, Z_DEFAULT_STRATEGY);
        if (ret != Z_OK)
        {
            throw (std::runtime_error("deflateInit2 failed while compressing."));
        }

        // For the compress
        zs.next_in = (Bytef*)str.data();
        zs.avail_in = str.size();           // set the z_stream's input

        char outbuffer[32768];

        // retrieve the compressed bytes blockwise
        do
        {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);

            ret = deflate(&zs, Z_FINISH);

            if (result.size() < zs.total_out)
            {
                // append the block to the output string
                result.append(outbuffer, zs.total_out - result.size());
            }
        }
        while (ret == Z_OK);

        deflateEnd(&zs);

        if (ret != Z_STREAM_END)            // an error occurred that was not EOF
        {
            std::ostringstream oss;
            oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
            throw (std::runtime_error(oss.str()));
        }

        return result;
    }

  private:
    CURL* const m_curl;
    struct curl_slist* m_headers = nullptr;
    std::string string_buffer;
};
