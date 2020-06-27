#include <crow/types/interfaces/thread.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const thread_value_t& thread_value)
{
    fill_if_not_empty("stacktrace", nlohmann::json(thread_value.stacktrace), j);
    fill_if_not_empty("id", thread_value.id, j);
    fill_if_not_empty("crashed", thread_value.crashed, j);
    fill_if_not_empty("current", thread_value.current, j);
    fill_if_not_empty("name", thread_value.name, j);
}

void to_json(nlohmann::json& j, const thread_t& thread)
{
    fill_if_not_empty("values", thread.values, j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
