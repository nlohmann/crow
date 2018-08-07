/*
 _____ _____ _____ _ _ _
|     | __  |     | | | |  Crow - a Sentry client for C++
|   --|    -|  |  | | | |  version 0.0.4
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

#ifndef NLOHMANN_CROW_UTILITIES_HPP
#define NLOHMANN_CROW_UTILITIES_HPP

#include <cstdint>
#include <string>
#include <thirdparty/json/json.hpp>

using json = nlohmann::json;

namespace nlohmann
{
namespace crow_utilities
{

json get_backtrace(int skip = 1);

/*!
 * @brief return pretty type name
 * @param[in] type_id_name result of type_id().name()
 * @param[in] only_module whether only the module name should be returned
 * @return demangled prettified name
 */
std::string pretty_name(const char* type_id_name,
                        bool only_module = false);

/*!
 * @brief get the seconds since epoch
 * @return seconds since epoch
 */
std::int64_t get_timestamp();

/*!
 * @brief get the current date and time as ISO 8601 string
 * @return ISO 8601 string
 */
std::string get_iso8601();

/*!
 * @brief generate a UUID4 without dashes
 * @return UUID4
 */
std::string generate_uuid();

}
}

#endif
