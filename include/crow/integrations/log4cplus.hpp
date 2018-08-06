#ifndef NLOHMANN_CROW_LOG4CPLUS_HPP
#define NLOHMANN_CROW_LOG4CPLUS_HPP

#include <iostream>
#include <unordered_map>
#include <crow/crow.hpp>
#include <crow/integrations/loggers.hpp>
#include <log4cplus/log4cplus.h>
#include <log4cplus/clogger.h>

namespace nlohmann
{
namespace crow_integrations
{

class crow_log4cplus_appender : public log4cplus::Appender
{
  public:
    explicit crow_log4cplus_appender(crow& client,
                                     const std::string& appender_name = "sentry appender",
                                     const std::unordered_map<log4cplus_loglevel_t, log_action>& config = std::unordered_map<log4cplus_loglevel_t, log_action>())
        : m_client(client)
    {
        setName(appender_name);
        m_log_actions[L4CP_FATAL_LOG_LEVEL] = log_action::message_fatal;
        m_log_actions[L4CP_ERROR_LOG_LEVEL] = log_action::message_error;
        m_log_actions[L4CP_WARN_LOG_LEVEL] = log_action::breadcrumb_warning;
        m_log_actions[L4CP_INFO_LOG_LEVEL] = log_action::breadcrumb_info;
        m_log_actions[L4CP_DEBUG_LOG_LEVEL] = log_action::breadcrumb_debug;
        m_log_actions[L4CP_TRACE_LOG_LEVEL] = log_action::breadcrumb_debug;

        for (const auto& config_entry : config)
        {
            m_log_actions[config_entry.first] = config_entry.second;
        }
    }

    ~crow_log4cplus_appender() override
    {
        // properly clean up appender
        destructorImpl();
    }

    void close() override {};

  protected:
    void append(const log4cplus::spi::InternalLoggingEvent& event) override
    {
        // look up action in configuration or use default action
        const auto lookup_action = m_log_actions.find(event.getLogLevel());
        const log_action action = (lookup_action != m_log_actions.end())
                                  ? lookup_action->second
                                  : message_warning;

        switch (action)
        {
            case message_fatal:
            case message_error:
            case message_warning:
            case message_info:
            case message_debug:
            {
                m_client.capture_message(event.getMessage(),
                {
                    {"logger", event.getLoggerName()},
                    {"level", log_action_level(action)},
                    {
                        "extra", {
                            {"location", event.getFile() + ":" + std::to_string(event.getLine())},
                            {"function", event.getFunction()}
                        }
                    }
                });
                return;
            }

            case breadcrumb_fatal:
            case breadcrumb_error:
            case breadcrumb_warning:
            case breadcrumb_info:
            case breadcrumb_debug:
            {
                m_client.add_breadcrumb(event.getMessage(),
                {
                    {"category", event.getLoggerName()},
                    {"level", log_action_level(action)},
                    {
                        "data", {
                            {"location", event.getFile() + ":" + std::to_string(event.getLine())},
                            {"function", event.getFunction()}
                        }
                    }
                });

                return;
            }

            case ignore:
                return;
        }
    }

  private:
    /// configuration of log actions per log level
    std::unordered_map<log4cplus_loglevel_t, log_action> m_log_actions;
    /// the associated Sentry client
    crow& m_client;
};


}
}

#endif
