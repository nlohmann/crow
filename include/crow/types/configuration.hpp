#pragma once

#include <chrono>
#include <functional>
#include <string>

namespace crow {
namespace types {

/*!
 * @brief SDK configuration
 *
 * @sa https://docs.sentry.io/error-reporting/configuration
 */
struct configuration
{
    /*!
     * @brief environment for events
     *
     * This string is freeform and not set by default. A release can be
     * associated with more than one environment to separate them in the UI
     * (think staging vs prod or similar). By default the SDK will try to read
     * this value from the `SENTRY_ENVIRONMENT` environment variable.
     */
    std::string environment{};

    /*!
     * @brief sample rate for events
     *
     * Configures the sample rate as a percentage of events to be sent in the
     * range of 0.0 to 1.0. The default is 1.0 which means that 100% of events
     * are sent. If set to 0.1 only 10% of events will be sent. Events are
     * picked randomly.
     */
    double sample_rate{1.0};

    /*!
     * @brief maximal number of breadcrumbs
     *
     * This variable controls the total amount of breadcrumbs that should be
     * captured. This defaults to 50.
     */
    std::size_t max_breadcrumbs{50};

    /*!
     * @brief whether to attach stack traces to all messages
     *
     * When enabled, stack traces are automatically attached to all messages
     * logged. Note that stack traces are always attached to exceptions but
     * when this is set stack traces are also sent with messages. This, for
     * instance, means that stack traces appear next to all log messages.
     *
     * It’s important to note that grouping in Sentry is different for events
     * with stack traces and without. This means that you will get new groups
     * as you enable or disable this flag for certain events.
     *
     * This feature is off by default.
     */
    bool attach_stacktrace{false};

    /*!
     * @brief timeout for HTTP calls
     *
     * After the timeout, a HTTP call will fail and the event is discarded.
     */
    std::chrono::milliseconds http_timeout{std::chrono::milliseconds{2000}};

    /*!
     * @brief function to decide whether a symbol name belongs to the app
     *
     * If no function is defined, only symbols starting with `std::`, `crow::`,
     * `backward::`, or `__` are treated as non-app symbols.
     *
     * In case the function returns true, the symbol is treated as external to
     * the app and is excluded from the stack trace by default.
     */
    std::function<bool(const std::string&)> symbol_is_external = nullptr;
};

} // namespace types
} // namespace crow
