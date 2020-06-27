# curl_wrapper

## About

This project provides a C++11 wrapper around [libcurl](https://curl.haxx.se/libcurl/) with a touch of Python's [requests](http://python-requests.org) library, powered by [JSON for Modern C++](https://github.com/nlohmann/json).

The project is originally part of [Crow](https://github.com/nlohmann/crow).

## Design Goals

- make HTTP requests as easy as calling a C++ function
- consume HTTP responses just like any C++ container
- natural JSON support for responses and POST payloads
- use C++ types where possible (e.g, `std::chrono` for timeouts)
- have all resources taken care of by RAII

## Example

<!-- embedme docs/assets/example.cpp -->
```cpp
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
```

## Features

- HTTP operations: GET, POST
- gzip compression for POST payloads
- POST payloads as `std::string` or JSON
- option: add timeouts
- option: switch on/off redirections
- option: switch on/off verbose logging to stderr

## Change Log

- version 0.3.0 (2020-01-13)
    - split code into header and source files to speed up building of client code
    - made initialization of libcurl thread-safe
    - added convenience constructor for `response` objects
    - switch off maintainer targets if project is included by other CMake project
    - made dependencies controllable via CMake switches
- version 0.2.0 (2019-11-15)
    - initial release

## License

[MIT License](http://opensource.org/licenses/MIT)

Copyright (c) 2018-2020 [Niels Lohmann](http://nlohmann.me/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
