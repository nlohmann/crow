#include <utility>
#include <crow/crow.hpp>
#include <crow/hub/hub.hpp>

namespace crow {

hub::hub(const crow::client& client, crow::scope&& scope, crow::types::configuration configuration)
    : m_client(client)
    , m_configuration(std::move(configuration))
{
    m_scopes.push(std::move(scope));
    assert(not m_scopes.empty());
}

void hub::capture_event(const types::event_t& event) const
{
    types::event_t e{event};
    std::lock_guard<std::mutex> lock(m_scopes_mutex);
    assert(not m_scopes.empty());
    m_scopes.top().apply_to_event(e);

    if (before_send_hook(e))
    {
        m_client.capture_event(e);
    }
}

void hub::capture_exception(const types::interfaces::exception_t& exception, types::event_t event) const
{
    std::lock_guard<std::mutex> lock(m_scopes_mutex);
    assert(not m_scopes.empty());
    m_scopes.top().apply_to_event(event);
    event.exception = exception;
    event.level = (exception.mechanism.handled.is_set() && exception.mechanism.handled.value()) ? types::level_t::error : types::level_t::fatal;

    if (before_send_hook(event))
    {
        m_client.capture_event(event);
    }
}

void hub::capture_message(const std::string& message, types::level_t level)
{
    types::event_t e;
    std::lock_guard<std::mutex> lock(m_scopes_mutex);
    assert(not m_scopes.empty());
    m_scopes.top().apply_to_event(e);
    e.level = level;
    e.message = message;

    if (before_send_hook(e))
    {
        m_client.capture_event(e);
    }
}

void hub::add_breadcrumb(const types::interfaces::breadcrumb_value_t& breadcrumb)
{
    std::lock_guard<std::mutex> lock(m_scopes_mutex);
    assert(not m_scopes.empty());
    m_scopes.top().add_breadcrumb(breadcrumb);
}

void hub::configure_scope(const std::function<void(scope&)>& callback)
{
    std::lock_guard<std::mutex> lock(m_scopes_mutex);
    assert(not m_scopes.empty());
    callback(m_scopes.top());
}

void hub::set_before_send_hook(const std::function<bool(crow::types::event_t& event)>& before_send_hook)
{
    std::lock_guard<std::mutex> lock(m_before_send_hook_mutex);
    m_before_send_hook = before_send_hook;
}

bool hub::before_send_hook(crow::types::event_t& event) const
{
    std::lock_guard<std::mutex> lock(m_before_send_hook_mutex);
    return m_before_send_hook ? m_before_send_hook(event) : true;
}

const crow::client& hub::get_client() const
{
    return m_client;
}

const crow::types::configuration& hub::get_configuration() const
{
    return m_configuration;
}

crow::client& hub::get_client()
{
    return m_client;
}

hub& hub::get_main()
{
    assert(crow::internal::main);
    return *crow::internal::main;
}

} // namespace crow
