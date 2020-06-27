#include <crow/types/interfaces/user.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const user_t& user)
{
    fill_if_not_empty("id", user.id, j);
    fill_if_not_empty("email", user.email, j);
    fill_if_not_empty("ip_address", user.ip_address, j);
    fill_if_not_empty("username", user.username, j);

    if (user.extra.is_object())
    {
        for (const auto& element : user.extra.items())
        {
            j[element.key()] = element.value();
        }
    }
}

} // namespace interfaces
} // namespace types
} // namespace crow
