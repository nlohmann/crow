#pragma once

#include <nlohmann/json.hpp>

namespace crow {
namespace types {

void fill_if_not_empty(const char* key, long value, nlohmann::json& j);

void fill_if_not_empty(const char* key, int value, nlohmann::json& j);

void fill_if_not_empty(const char* key, std::size_t value, nlohmann::json& j);

void fill_if_not_empty(const char* key, double value, nlohmann::json& j);

template <typename T>
void fill_if_not_empty(const char* key, const T& value, nlohmann::json& j)
{
    if (not value.empty())
    {
        j[key] = value;
    }
}

} // namespace types
} // namespace crow
