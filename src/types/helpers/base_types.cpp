#include <crow/types/helpers/base_types.hpp>

namespace crow {
namespace types {

void to_json(nlohmann::json& j, const level_t& level)
{
    switch (level)
    {
        case level_t::fatal:
            j = "fatal";
            break;
        case level_t::error:
            j = "error";
            break;
        case level_t::warning:
            j = "warning";
            break;
        case level_t::info:
            j = "info";
            break;
        case level_t::debug:
            j = "debug";
            break;
    }
}

void to_json(nlohmann::json& j, const optional_bool& b)
{
    if (b.is_set())
    {
        j = b.value();
    }
}

} // namespace types
} // namespace crow
