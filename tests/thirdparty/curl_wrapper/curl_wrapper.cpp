#include <thirdparty/curl_wrapper/curl_wrapper.hpp>

size_t curl_wrapper::message_count = 0;
json curl_wrapper::results = json::array();
