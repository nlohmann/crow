#pragma once

#include <string>
#include <unordered_map>
#include <crow/crow.hpp>
#include <crow/integrations/loggers.hpp>
#include <log4cplus/clogger.h>
#include <log4cplus/log4cplus.h>

/*!
 * @file log4cplus.hpp
 * @brief interface for the Log4cplus appender
 */

namespace nlohmann {
namespace crow_integrations {

/*!
 * @example log4cplus/example.cpp
 */
/*!
 * @brief appender for Log4cplus
 * @since 0.0.4
*/
class log4cplus_appender : public log4cplus::Appender
{
  public:
    /*!
     * @brief create the appender
     * @param[in] client the Crow client to use
     * @param[in] appender_name the name of the appender (used internally of Log4cplus, defaults to "sentry appender")
     * @param[in] config a configuration (see below)
     * @since 0.0.4
     */
    explicit log4cplus_appender(const std::string& appender_name = "sentry appender",
                                const std::unordered_map<log4cplus_loglevel_t, log_action>& config = std::unordered_map<log4cplus_loglevel_t, log_action>())
    {
        setName(appender_name);
        m_log_actions[L4CP_FATAL_LOG_LEVEL] = log_action::message_fatal;
        m_log_actions[L4CP_ERROR_LOG_LEVEL] = log_action::message_error;
        m_log_actions[L4CP_WARN_LOG_LEVEL] = log_action::breadcrumb_warning;
        m_log_actions[L4CP_INFO_LOG_LEVEL] = log_action::breadcrumb_info;
        m_log_actions[L4CP_DEBUG_LOG_LEVEL] = log_action::ignore;
        m_log_actions[L4CP_TRACE_LOG_LEVEL] = log_action::ignore;

        for (const auto& config_entry : config)
        {
            m_log_actions[config_entry.first] = config_entry.second;
        }
    }

    /*!
     * @brief clean up appender state
     * @since 0.0.4
     */
    ~log4cplus_appender() override
    {
        // properly clean up appender
        destructorImpl();
    }

    /*!
     * @brief closing the appender
     * @since 0.0.4
     */
    void close() override{};

  protected:
    /*!
     * @brief the actual appender implementation
     * @param[in] logging_event log logging_event to append
     * @since 0.0.4
     */
    void append(const log4cplus::spi::InternalLoggingEvent& logging_event) override
    {
        // look up action in configuration or use default action
        const auto lookup_action = m_log_actions.find(logging_event.getLogLevel());
        const log_action action = (lookup_action != m_log_actions.end())
                                      ? lookup_action->second
                                      : message_warning;

        if (action == ignore)
        {
            return;
        }

        // add breadcrumb
        crow::types::interfaces::breadcrumb_value_t breadcrumb{};
        breadcrumb.level = get_level(action);
        breadcrumb.category = logging_event.getLoggerName();
        breadcrumb.data["location"] = logging_event.getFile() + ":" + std::to_string(logging_event.getLine());
        breadcrumb.data["function"] = logging_event.getFunction();
        breadcrumb.data["TID"] = logging_event.getThread2();
        breadcrumb.message = logging_event.getMessage();
        crow::add_breadcrumb(breadcrumb);

        switch (action)
        {
            case message_fatal:
            case message_error:
            case message_warning:
            case message_info:
            case message_debug:
            {
                crow::capture_message(logging_event.getMessage(), get_level(action));
                return;
            }

            default:
                return;
        }
    }

  private:
    /// configuration of log actions per log level
    std::unordered_map<log4cplus_loglevel_t, log_action> m_log_actions;
};

} // namespace crow_integrations
} // namespace nlohmann
