#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {

void fill_if_not_empty(const char* key, const long value, nlohmann::json& j)
{
    if (value > 0)
    {
        j[key] = value;
    }
}

void fill_if_not_empty(const char* key, const int value, nlohmann::json& j)
{
    if (value > 0)
    {
        j[key] = value;
    }
}

void fill_if_not_empty(const char* key, const std::size_t value, nlohmann::json& j)
{
    if (value > 0)
    {
        j[key] = value;
    }
}

void fill_if_not_empty(const char* key, const double value, nlohmann::json& j)
{
    if (value > 0)
    {
        j[key] = value;
    }
}

} // namespace types
} // namespace crow
