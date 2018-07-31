# Crow - a C++ client for Sentry

[![Build Status](https://travis-ci.org/nlohmann/crow.svg?branch=master)](https://travis-ci.org/nlohmann/crow)
[![Build status](https://ci.appveyor.com/api/projects/status/61gmcsom8dcrjxpn/branch/master?svg=true)](https://ci.appveyor.com/project/nlohmann/crow/branch/master)
[![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)](http://nlohmann.github.io/crow)

[**Sentry**](https://sentry.io/welcome/) is such a useful tool, but there is currently [no official support](https://sentry.io/platforms/) for C++. **Crow** is an official Sentry client for C++ that tries to fill this gap to allow for Sentry notifications in long-running C++ applications where you do not want to constantly look at log files.

## API

- `crow::crow(dsn, attributes=null, install_handlers=true)` to create a client
- `crow::capture_message(message, options=nullptr, async=true)` to send a message
- `crow::capture_exception(exception, async=true, handled=true)` to send an exception
- `crow::add_breadcrumb(message, type="default")` to add a breadcrumb
- `crow::get_last_event_id()` to get the id of the last event

See [the documentation](https://nlohmann.github.io/crow/classnlohmann_1_1crow.html) for a complete overview of the public API.

## Requirements for Sentry SDKs

The following items from [the SDK implementation guidelines](https://docs.sentry.io/clientdev/overview/#writing-an-sdk) are realized at the moment:

- The following items are expected of production-ready SDKs:
    - [x] DSN configuration
    - [ ] Graceful failures (e.g. Sentry server is unreachable)
    - [ ] Setting attributes (e.g. tags and extra data)
    - [x] Support for Linux, Windows and OS X (where applicable)
- Feature based support is required for the following:
    - [ ] If cookie data is available, it’s not sent by default
    - [ ] If POST data is available, it’s not sent by default
- Additionally, the following features are highly encouraged:
    - [x] Automated error capturing (e.g. uncaught exception handlers)
    - [ ] Logging framework integration
    - [x] Non-blocking event submission
    - [ ] Basic data sanitization (e.g. filtering out values that look like passwords)
    - [x] Context data helpers (e.g. setting the current user, recording breadcrumbs)
    - [ ] Event sampling
    - [ ] Honor Sentry’s HTTP 429 Retry-After header
    - [ ] Pre and Post event send hooks
    - [ ] Local variable values in stacktrace (on platforms where this is possible)

## Change Log

### Version 0.0.2

- :construction_worker: integrated Travis and AppVeyor CI (#1)
- :memo: added Doxygen documentation as GitHub page (#4)
- :sparkles: added function to retrieve last event id (#8)
- :sparkles: collecting information on user, machine, operating system, and compiler
- :wheelchair: client now compiles with macOS, Linux, Windows (MinGW and MSVC)
- :heavy_minus_sign: removed Sole dependency for UUID generation
- :heavy_minus_sign: removed Date dependency for ISO8601 date string generation
- :white_check_mark: adding unit tests and live test client

### Version 0.0.1

- :tada: initial release

## Prerequisites

You need [libcurl](https://curl.haxx.se/libcurl/) and a C++11 compiler to use the library. Curl should be detected by CMake. If this does not work, you can [download libcurl](https://curl.haxx.se/download.html) and pass the path to the source release folder to CMake via `-DCROW_EXTERNAL_CURL_PROJECT=curl-7.61.0`. This libcurl is then built and statically linked.

## License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2018 [Niels Lohmann](http://nlohmann.me)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Used third-party tools

The library itself consists of a single header file licensed under the MIT license. However, it is built, tested, documented, and whatnot using a lot of third-party tools and services. Thanks a lot!

- [**Artistic Style**](http://astyle.sourceforge.net) for automatic source code identation
- [**Cmake**](https://cmake.org) for build automation
- [**Doxygen**](http://www.stack.nl/~dimitri/doxygen/) to generate documentation
- [**JSON for Modern C++**](https://github.com/nlohmann/json) to use JSON datatypes inside C++
- [**libcurl**](https://curl.haxx.se/libcurl/) for HTTP communication with the Sentry servers
