#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <crow/hub/hub.hpp>
#include <crow/types/configuration.hpp>

namespace crow {

/// @name initialization
/// @{

/*!
 * @brief configures the Sentry SDK
 *
 * @param[in] dsn The DSN tells the SDK where to send the events to. If this
 *            value is not provided, the SDK will try to read it from the
 *            `SENTRY_DSN` environment variable. If that variable also does not
 *            exist, the SDK will just not send any events.
 *
 * @param[in] configuration additional configuration options
 */
extern void init(const std::string& dsn = "",
                 const types::configuration& configuration = {});

/// @}

/// @name handlers
/// @{

/*!
 * @brief install signal and termination handlers
 *
 * Installs both signal handlers and termination handlers.
 *
 * @sa @ref install_signal_handlers()
 * @sa @ref install_termination_handler()
 *
 * @note This function is thread-safe.
 */
extern void install_handlers();

/*!
 * @brief install signal handlers
 *
 * Installs signal handlers (@ref crow::utils::signal_handler()) for the
 * signals SIGTERM, SIGSEGV, SIGINT, SIGILL, SIGABRT, and SIGFPE. This signal
 * handler creates Sentry event from the signal and calls the previously
 * installed signal handler, which is equivalent to calling `std::abort` in
 * case no user-defined signal handler was installed for that signal.
 *
 * @sa @ref crow::utils::signal_handler()
 *
 * @note This function is thread-safe.
 */
extern void install_signal_handlers();

/*!
 * @brief install termination handler
 *
 * Installs a termination handler (@ref crow::utils::termination_handler())
 * that detects uncaught exceptions, creates a Sentry event from it and passes
 * them to the previously installed termination handler, which is equivalent to
 * calling `std::abort()` in case no user-defined termination handler was
 * installed.
 *
 * @sa @ref crow::utils::termination_handler()
 *
 * @note This function is thread-safe.
 */
extern void install_termination_handler();

/// @}

/// @name event captures
/// @{

/*!
 * @brief capture an event
 * @param event event to capture
 * @note This function is thread-safe.
 */
extern void capture_event(const types::event_t& event);

/*!
 * @brief capture an exception
 * @param exception exception to capture
 *
 * @sa capture_exception(const std::exception&, bool handled);
 *
 * @note This function is thread-safe.
 */
extern void capture_exception(const types::interfaces::exception_t& exception);

/*!
 * @brief capture an exception
 * @param exception exception to capture
 * @param handled whether the exception was handled by a `try`/`catch` block (default: true)
 * @param event optional pre-filled event
 *
 * @sa capture_exception(const types::interface::exception_t&)
 *
 * @note This function is thread-safe.
 */
extern void capture_exception(const std::exception& exception, bool handled = true, types::event_t event = {});

/*!
 * @brief capture a signal
 * @param signal signal to capture
 * @param handled whether the signal was handled (default: true)
 * @param event optional pre-filled event
 * @note This function is thread-safe.
 */
extern void capture_signal(int signal, bool handled = true, types::event_t event = {});

/*!
 * @brief capture the current errno
 * @note This is a noop in case `errno` is 0.
 * @note This function is thread-safe.
 */
extern void capture_errno();

/*!
 * @brief capture a message
 * @param message message to capture
 * @param level the level for the message
 * @note This function is thread-safe.
 */
extern void capture_message(const std::string& message, types::level_t level);

/*!
 * @brief add a breadcrumb
 * @param breadcrumb breadcrumb to add
 * @note This function is thread-safe.
 */
extern void add_breadcrumb(const types::interfaces::breadcrumb_value_t& breadcrumb);

/// @}

/// @name misc
/// @{

/*!
 * @brief configure the current scope
 */
extern void configure_scope(const std::function<void(scope&)>& callback);

/*!
 * @brief set a callback function to manipulate or filter events before sending
 *
 * This function is called with an SDK specific event object and can return a
 * modified event object or nothing to skip reporting the event. This can be
 * used for instance for manual PII stripping before sending.
 *
 * @note This function is thread-safe.
 */
extern void set_before_send_hook(const std::function<bool(crow::types::event_t& event)>& before_send_hook);

/// @}

namespace internal {

extern std::unique_ptr<hub> main;
extern std::terminate_handler existing_termination_handler;

} // namespace internal

} // namespace crow
