#pragma once

#include <nlohmann/json.hpp>

namespace crow {
namespace types {

enum class level_t
{
    fatal,
    error,
    warning,
    info,
    debug
};

void to_json(nlohmann::json& j, const level_t& level);

class optional_bool
{
  public:
    optional_bool() = default;
    optional_bool(bool value)
        : m_value(value), m_set(true) {}

    optional_bool& operator=(bool value)
    {
        m_value = value;
        m_set = true;
        return *this;
    }

    bool value() const noexcept
    {
        return m_value;
    }

    bool is_set() const noexcept
    {
        return m_set;
    }

    bool empty() const noexcept
    {
        return not m_set;
    }

  private:
    bool m_value{false};
    bool m_set{false};
};

void to_json(nlohmann::json& j, const optional_bool& b);

} // namespace types
} // namespace crow
