#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <stack>
#include <crow/client/client.hpp>
#include <crow/hub/scope.hpp>
#include <crow/types/configuration.hpp>

namespace crow {

class hub
{
  public:
    hub(const crow::client& client, crow::scope&& scope, crow::types::configuration = {});

    void capture_event(const types::event_t& event) const;
    void capture_exception(const types::interfaces::exception_t& exception, types::event_t event = {}) const;
    void capture_message(const std::string& message, types::level_t level);
    void add_breadcrumb(const types::interfaces::breadcrumb_value_t& breadcrumb);
    void configure_scope(const std::function<void(scope&)>& callback);
    void set_before_send_hook(const std::function<bool(crow::types::event_t& event)>& before_send_hook);

    crow::client& get_client();
    const crow::client& get_client() const;
    const crow::types::configuration& get_configuration() const;
    static hub& get_main();

  private:
    bool before_send_hook(crow::types::event_t& event) const;

    crow::client m_client;
    std::stack<crow::scope> m_scopes;
    std::function<bool(crow::types::event_t& event)> m_before_send_hook = nullptr;
    crow::types::configuration m_configuration;

    mutable std::mutex m_before_send_hook_mutex;
    mutable std::mutex m_scopes_mutex;
};

} // namespace crow
