#include <crow/types/interfaces/exception.hpp>
#include <crow/utils/utils.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const mach_exception_t& mach_exception)
{
    fill_if_not_empty("exception", mach_exception.exception, j);
    fill_if_not_empty("code", mach_exception.code, j);
    fill_if_not_empty("subcode", mach_exception.subcode, j);
    fill_if_not_empty("name", mach_exception.name, j);
}

void to_json(nlohmann::json& j, const signal_t& signal)
{
    fill_if_not_empty("number", signal.number, j);
    fill_if_not_empty("code", signal.code, j);
    fill_if_not_empty("name", signal.name, j);
    fill_if_not_empty("code_name", signal.code_name, j);
}

void to_json(nlohmann::json& j, const errno_t& errno_)
{
    fill_if_not_empty("number", errno_.number, j);
    fill_if_not_empty("name", errno_.name, j);
}

void to_json(nlohmann::json& j, const meta_t& meta)
{
    fill_if_not_empty("signal", nlohmann::json(meta.signal), j);
    fill_if_not_empty("mach_exception", nlohmann::json(meta.mach_exception), j);
    fill_if_not_empty("errno", nlohmann::json(meta.errno_), j);
}

void to_json(nlohmann::json& j, const mechanism_t& mechanism)
{
    fill_if_not_empty("type", mechanism.type, j);
    fill_if_not_empty("description", mechanism.description, j);
    fill_if_not_empty("help_link", mechanism.help_link, j);
    fill_if_not_empty("handled", mechanism.handled, j);
    fill_if_not_empty("meta", nlohmann::json(mechanism.meta), j);
    fill_if_not_empty("data", mechanism.data, j);
}

void to_json(nlohmann::json& j, const exception_t& exception)
{
    fill_if_not_empty("type", exception.type, j);
    fill_if_not_empty("value", exception.value, j);
    fill_if_not_empty("module", exception.module, j);
    fill_if_not_empty("thread_id", exception.thread_id, j);
    fill_if_not_empty("mechanism", nlohmann::json(exception.mechanism), j);
    fill_if_not_empty("stacktrace", nlohmann::json(exception.stacktrace), j);
}

exception_t::exception_t(const std::exception& exception, bool handled)
{
    auto type_name = crow::utils::pretty_name(typeid(exception).name(), false);
    value = exception.what();
    type = type_name + " exception";
    stacktrace = crow::utils::get_backtrace();
    mechanism.type = "exception";

    auto info = utils::exception_information(type_name);
    mechanism.description = info.description;
    mechanism.help_link = info.url;
    mechanism.handled = handled;
}

exception_t::exception_t(int signal, bool handled)
{
    const auto information = crow::utils::signal_information(signal);
    mechanism.type = "signal";
    mechanism.description = "A signal has been received.";
    mechanism.help_link = "https://en.wikipedia.org/wiki/Signal_(IPC)";
    mechanism.meta.signal.number = signal;
    mechanism.meta.signal.name = information.name;
    mechanism.meta.signal.code = signal;
    mechanism.handled = handled;
    type = information.name;
    value = information.description;
    stacktrace = crow::utils::get_backtrace();
}

} // namespace interfaces
} // namespace types
} // namespace crow
