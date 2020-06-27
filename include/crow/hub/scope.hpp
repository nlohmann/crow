#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <crow/types/configuration.hpp>
#include <crow/types/event.hpp>

namespace crow {

class scope
{
  public:
    explicit scope(const crow::types::configuration& configuration);

    void set_user(const types::interfaces::user_t& user);
    void remove_user();

    void set_environment(const std::string& environment);
    void remove_environment();

    void set_extra(const std::string& key, const nlohmann::json& value);
    void remove_extra(const std::string& key);
    void set_extras(const nlohmann::json& extras);

    void set_tag(const std::string& key, const std::string& value);
    void remove_tag(const std::string& key);
    void set_tags(const std::unordered_map<std::string, std::string>& tags);

    void set_level(types::level_t level);
    void set_transaction(const std::string& transaction);
    void set_fingerprint(const std::vector<std::string>& fingerprint);
    void clear();
    void add_breadcrumb(const types::interfaces::breadcrumb_value_t& breadcrumb);
    void clear_breadcrumbs();

    void apply_to_event(types::event_t& event) const;

  private:
    types::interfaces::user_t m_user;
    std::string m_environment;
    nlohmann::json m_extra;
    std::unordered_map<std::string, std::string> m_tags;
    types::level_t m_level{};
    std::string m_transaction;
    std::vector<std::string> m_fingerprint;
    std::list<types::interfaces::breadcrumb_value_t> m_breadcrumbs;
    const crow::types::configuration m_configuration;

    mutable std::unique_ptr<std::mutex> m_breadcrumbs_mutex{new std::mutex()};
};

} // namespace crow
