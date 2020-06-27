#include <crow/hub/scope.hpp>
#include <crow/utils/utils.hpp>

namespace crow {

scope::scope(const crow::types::configuration& configuration)
    : m_configuration(configuration)
{}

void scope::set_user(const types::interfaces::user_t& user)
{
    m_user = user;
}

void scope::remove_user()
{
    m_user = {};
}

void scope::set_environment(const std::string& environment)
{
    m_environment = environment;
}

void scope::remove_environment()
{
    m_environment.clear();
}

void scope::set_extra(const std::string& key, const nlohmann::json& value)
{
    m_extra[key] = value;
}

void scope::remove_extra(const std::string& key)
{
    m_extra.erase(key);
}

void scope::set_extras(const nlohmann::json& extras)
{
    m_extra = extras;
}

void scope::set_tag(const std::string& key, const std::string& value)
{
    m_tags[key] = value;
}

void scope::remove_tag(const std::string& key)
{
    m_tags.erase(key);
}

void scope::set_tags(const std::unordered_map<std::string, std::string>& tags)
{
    m_tags = tags;
}

void scope::set_level(const types::level_t level)
{
    m_level = level;
}

void scope::set_transaction(const std::string& transaction)
{
    m_transaction = transaction;
}

void scope::set_fingerprint(const std::vector<std::string>& fingerprint)
{
    m_fingerprint = fingerprint;
}

void scope::clear()
{
    m_user = {};
    m_environment.clear();
    m_extra.clear();
    m_tags.clear();
    m_level = {};
    m_transaction.clear();
    m_fingerprint.clear();

    std::lock_guard<std::mutex> lock(*m_breadcrumbs_mutex);
    m_breadcrumbs.clear();
}

void scope::add_breadcrumb(const types::interfaces::breadcrumb_value_t& breadcrumb)
{
    std::lock_guard<std::mutex> lock(*m_breadcrumbs_mutex);
    if (breadcrumb.timestamp.empty() and breadcrumb.timestamp_unix == -1)
    {
        types::interfaces::breadcrumb_value_t b{breadcrumb};
        b.timestamp = crow::utils::get_iso8601();
        m_breadcrumbs.push_back(b);
    }
    else
    {
        m_breadcrumbs.push_back(breadcrumb);
    }

    // enforce upper bound of breadcrumbs
    if (m_breadcrumbs.size() > m_configuration.max_breadcrumbs)
    {
        const auto delete_count = m_breadcrumbs.size() - m_configuration.max_breadcrumbs;
        m_breadcrumbs.erase(m_breadcrumbs.begin(), std::next(m_breadcrumbs.begin(), delete_count));
    }
}

void scope::clear_breadcrumbs()
{
    std::lock_guard<std::mutex> lock(*m_breadcrumbs_mutex);
    m_breadcrumbs.clear();
}

void scope::apply_to_event(types::event_t& event) const
{
    event.user = m_user;
    event.environment = m_environment;
    if (m_extra.is_object())
    {
        event.extra.update(m_extra);
    }
    event.tags = m_tags;
    // TODO event.level = m_level;
    event.transaction = m_transaction;
    event.fingerprint = m_fingerprint;

    std::lock_guard<std::mutex> lock(*m_breadcrumbs_mutex);
    event.breadcrumbs.values = m_breadcrumbs;
}

} // namespace crow
