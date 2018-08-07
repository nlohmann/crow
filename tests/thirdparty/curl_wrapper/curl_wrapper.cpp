#include <thirdparty/curl_wrapper/curl_wrapper.hpp>

size_t curl_wrapper::message_count = 0;
json curl_wrapper::m_results = json::array();
std::mutex curl_wrapper::m_results_mutex;
