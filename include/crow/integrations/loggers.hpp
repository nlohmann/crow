#pragma once

/*!
 * @file loggers.hpp
 * @brief common types for logger integrations
 */

namespace nlohmann {
/*!
 * @brief integrations of Crow into other frameworks
 */
namespace crow_integrations {

/*!
 * @brief different actions to be executed for a log event
 * @since 0.0.4
 */
enum log_action
{
    message_fatal,      ///< send a message with level "fatal"
    message_error,      ///< send a message with level "error"
    message_warning,    ///< send a message with level "warning"
    message_info,       ///< send a message with level "info"
    message_debug,      ///< send a message with level "debug"
    breadcrumb_fatal,   ///< add a breadcrumb with level "fatal"
    breadcrumb_error,   ///< add a breadcrumb with level "error"
    breadcrumb_warning, ///< add a breadcrumb with level "warning"
    breadcrumb_info,    ///< add a breadcrumb with level "info"
    breadcrumb_debug,   ///< add a breadcrumb with level "debug"
    ignore              ///< do nothing
};

/*!
 * @brief get Sentry level for each log action
 * @param action log action
 * @return string representation of Sentry level
 * @since 0.0.4
 */
const char* log_action_level(const log_action action) noexcept
{
    switch (action)
    {
        case message_fatal:
        case breadcrumb_fatal:
            return "fatal";
        case message_error:
        case breadcrumb_error:
            return "error";
        case message_warning:
        case breadcrumb_warning:
            return "warning";
        case message_info:
        case breadcrumb_info:
            return "info";
        case message_debug:
        case breadcrumb_debug:
            return "debug";
        default:
            return "";
    }
}

crow::types::level_t get_level(const log_action action) noexcept
{
    switch (action)
    {
        case message_fatal:
        case breadcrumb_fatal:
            return crow::types::level_t::fatal;

        case message_error:
        case breadcrumb_error:
            return crow::types::level_t::error;

        case message_warning:
        case breadcrumb_warning:
            return crow::types::level_t::warning;

        case message_info:
        case breadcrumb_info:
            return crow::types::level_t::info;

        case message_debug:
        case breadcrumb_debug:
            return crow::types::level_t::debug;

        default:
            return crow::types::level_t::info;
    }
}

} // namespace crow_integrations
} // namespace nlohmann
