/*
 _____ _____ _____ _ _ _
|     | __  |     | | | |  Crow - a Sentry client for C++
|   --|    -|  |  | | | |  version 0.0.5
|_____|__|__|_____|_____|  https://github.com/nlohmann/crow

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2018 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*!
 * @file crow_utilities.cpp
 * @brief implementation of Crow helper functions
 */

#include <chrono>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <typeinfo>
#include <src/crow_config.hpp>
#include <src/crow_utilities.hpp>

#ifndef NLOHMANN_CROW_MINGW
    #include <random>
#else
    #include <cstdlib> // for srand, rand
    #include <ctime> // for time
#endif

#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
    #include <cxxabi.h> // for abi::__cxa_demangle
#endif

#ifdef NLOHMANN_CROW_HAVE_EXECINFO_H
    #include <execinfo.h> // for backtrace
#endif

#ifdef NLOHMANN_CROW_HAVE_DLFCN_H
    #include <dlfcn.h> // for dladdr
#endif

namespace nlohmann
{

namespace crow_utilities
{

// https://gist.github.com/fmela/591333
// This function produces a stack backtrace with demangled function & method names.
json get_backtrace(int skip)
{
    json result = json::array();

#if defined(NLOHMANN_CROW_HAVE_DLFCN_H) && defined(NLOHMANN_CROW_HAVE_EXECINFO_H)
    void* callstack[128];
    const int nMaxFrames = sizeof(callstack) / sizeof(callstack[0]);
    char buf[1024];
    int nFrames = backtrace(callstack, nMaxFrames);
    char** symbols = backtrace_symbols(callstack, nFrames);

    for (int i = skip; i < nFrames; i++)
    {
        Dl_info info;
        if (dladdr(callstack[i], &info) && info.dli_sname)
        {
            char* demangled = nullptr;
            int status = -1;
            if (info.dli_sname[0] == '_')
            {
#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
                demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
#endif
            }

            const std::string function_name = (status == 0 ? demangled : info.dli_sname == nullptr ? symbols[i] : info.dli_sname);

            snprintf(buf, sizeof(buf), "%-3d %*p %s + %zd\n",
                     i, int(2 + sizeof(void*) * 2), callstack[i],
                     function_name.c_str(),
                     (char*) callstack[i] - (char*) info.dli_saddr);
            //std::cout << buf << std::endl;

            json entry;
            entry["function"] = function_name;

            if (not function_name.empty())
            {
                if (function_name.substr(0, 5) == "std::" or function_name.substr(0, 2) == "__")
                {
                    entry["in_app"] = false;
                }
            }

            result.push_back(entry);

            free(demangled);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%-3d %*p %s\n",
                     i, int(2 + sizeof(void*) * 2), callstack[i], symbols[i]);
        }
    }
    free(symbols);
#endif

    return result;
}

std::string pretty_name(const char* type_id_name,
                        const bool only_module)
{
#ifdef NLOHMANN_CROW_HAVE_CXXABI_H
    int status;
    std::string result = abi::__cxa_demangle(type_id_name, nullptr, nullptr, &status);
#else
    std::string result = type_id_name;
#endif

    return only_module
           ? result.substr(0, result.find_first_of(':'))
           : result;
}

std::int64_t get_timestamp()
{
    const auto tp = std::chrono::system_clock::now();
    const auto dur = tp.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(dur).count();
}

std::string get_iso8601()
{
    std::time_t now;
    std::time(&now);
    char buf[sizeof "2011-10-08T07:07:09Z"];
    std::strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now));
    return buf;
}

int get_random_number(int lower, int upper)
{
#ifdef NLOHMANN_CROW_MINGW
    static bool init = false;
    if (not init)
    {
        init = true;
        std::srand(std::time(nullptr));
    }
    return std::rand() % upper + lower;
#else
    std::random_device random_device;
    std::default_random_engine random_engine(random_device());
    std::uniform_int_distribution<int> uniform_dist(lower, upper);
    return uniform_dist(random_engine);
#endif
}

std::string generate_uuid()
{
    std::string result(32, ' ');

    for (std::size_t i = 0; i < result.size(); ++i)
    {
        // the 12th character must be a '4'
        if (i == 12)
        {
            result[i] = '4';
        }
        else
        {
            // get a random number from 0..15
            const auto r = static_cast<char>(get_random_number(0, 15));
            if (r < 10)
            {
                result[i] = '0' + r;
            }
            else
            {
                result[i] = 'a' + r - static_cast<char>(10);
            }
        }
    }

    return result;
}

}
}