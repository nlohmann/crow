#pragma once

#include <string>
#include <crow/crow.hpp>
#include <crow/types/interfaces/exception.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace crow {
namespace utils {

struct exception_information_t
{
    std::string description;
    std::string url;
};

struct signal_information_t
{
    std::string name;
    std::string description;
};

exception_information_t exception_information(const std::string& name);
signal_information_t signal_information(int signal);
char const* errnoname(int errno_);

std::string pretty_name(const char* type_id_name,
                        bool only_module);

int get_process_id() noexcept;

std::string thread_name();
std::size_t thread_id();

crow::types::interfaces::stacktrace_t get_backtrace(int skip = 0,
                                                    std::function<bool(const std::string&)> symbol_is_external = crow::internal::main ? crow::internal::main->get_configuration().symbol_is_external : nullptr);

void termination_handler();
void signal_handler(int signal);

int get_random_number(int lower, int upper);
std::string generate_uuid();
std::int64_t get_timestamp();
std::string get_iso8601();

json get_environment_variables();

std::string get_environment_variable(const char* name);

} // namespace utils
} // namespace crow
