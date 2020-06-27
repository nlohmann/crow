#include <http/http.hpp>

int main()
{
    http::request r;
    
    // GET request
    auto response = r.get("https://httpbin.org/anything");
        
    // access status code
    int code = response.status_code; // 200
    std::string reason = response.reason; // OK
    
    // throw in case the response was not successful
    response.throw_for_status();

    // access raw response
    std::string text = response.text; // {"args: {}, "headers": {"Accept": ...
    
    // access JSON response
    nlohmann::json j = response.json();
    
    // POST request with JSON payload
    auto response2 = r.post("https://httpbin.org/anything", j);
}
