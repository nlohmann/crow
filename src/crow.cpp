#include <cerrno>
#include <csignal>
#include <memory>
#include <mutex>
#include <utility>
#include <crow/crow.hpp>
#include <crow/hub/scope.hpp>
#include <crow/utils/utils.hpp>

namespace crow {

namespace internal {

std::unique_ptr<hub> main;
std::terminate_handler existing_termination_handler = nullptr;
std::mutex existing_termination_handler_mutex;

} // namespace internal

void init(const std::string& dsn, const types::configuration& configuration)
{
    crow::scope s(configuration);
    s.set_environment(configuration.environment);

    crow::internal::main.reset(new crow::hub(crow::client(dsn, configuration), std::move(s), configuration));
}

void install_handlers()
{
    install_signal_handlers();
    install_termination_handler();
}

void install_signal_handlers()
{
    // note: std::signal is thread-safe (POSIX requirement)
    std::signal(SIGTERM, &utils::signal_handler);
    std::signal(SIGSEGV, &utils::signal_handler);
    std::signal(SIGINT, &utils::signal_handler);
    std::signal(SIGILL, &utils::signal_handler);
    std::signal(SIGABRT, &utils::signal_handler);
    std::signal(SIGFPE, &utils::signal_handler);
}

void install_termination_handler()
{
    std::lock_guard<std::mutex> lock(internal::existing_termination_handler_mutex);
    internal::existing_termination_handler = std::get_terminate();

    // note: std::set_terminate is thread-safe (C++11 requirement)
    std::set_terminate(&utils::termination_handler);
}

void capture_event(const types::event_t& event)
{
    if (internal::main)
    {
        hub::get_main().capture_event(event);
    }
}

void capture_exception(const types::interfaces::exception_t& exception)
{
    if (internal::main)
    {
        hub::get_main().capture_exception(exception);
    }
}

void capture_exception(const std::exception& exception, const bool handled, types::event_t event)
{
    if (internal::main)
    {
        types::interfaces::exception_t e{exception, handled};
        hub::get_main().capture_exception(e, std::move(event));
    }
}

void capture_errno()
{
    if (internal::main and errno != 0)
    {
        const auto copy_errno = errno;

        types::interfaces::exception_t exception{};
        exception.value = std::strerror(copy_errno);
        exception.type = utils::errnoname(copy_errno);
        exception.stacktrace = crow::utils::get_backtrace();
        exception.thread_id = crow::utils::thread_id();
        exception.mechanism.type = "errno";
        exception.mechanism.meta.errno_.number = copy_errno;
        exception.mechanism.meta.errno_.name = std::strerror(copy_errno);
        exception.mechanism.help_link = "https://en.cppreference.com/w/cpp/error/errno_macros";
        exception.mechanism.description = "A previous call set errono to a non-zero value.";

        types::event_t event;
        event.extra["TID"] = crow::utils::thread_id();

        hub::get_main().capture_exception(exception, event);
        errno = copy_errno;
    }
}

void capture_signal(int signal, bool handled, types::event_t event)
{
    if (internal::main)
    {
        types::interfaces::exception_t exception(signal, handled);
        hub::get_main().capture_exception(exception, std::move(event));
    }
}

void capture_message(const std::string& message, types::level_t level)
{
    if (internal::main)
    {
        hub::get_main().capture_message(message, level);
    }
}

void add_breadcrumb(const types::interfaces::breadcrumb_value_t& breadcrumb)
{
    if (internal::main)
    {
        hub::get_main().add_breadcrumb(breadcrumb);
    }
}

void configure_scope(const std::function<void(scope&)>& callback)
{
    if (internal::main)
    {
        hub::get_main().configure_scope(callback);
    }
}

void set_before_send_hook(const std::function<bool(crow::types::event_t& event)>& before_send_hook)
{
    if (internal::main)
    {
        hub::get_main().set_before_send_hook(before_send_hook);
    }
}

} // namespace crow
