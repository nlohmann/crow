#pragma once

#include <string>
#include <vector>
#include <crow/types/helpers/helpers.hpp>
#include <crow/types/interfaces/stacktrace.hpp>

namespace crow {
namespace types {
namespace interfaces {

class thread_value_t
{
  public:
    /// you can also optionally bind a stack trace to the thread
    stacktrace_t stacktrace;

    /// the ID of the thread
    std::size_t id;

    /// an optional bool to indicate that the thread crashed
    optional_bool crashed;

    /// an optional bool to indicate that the thread was in the foreground
    optional_bool current;

    /// an optional thread name
    std::string name;
};

void to_json(nlohmann::json& j, const thread_value_t& thread_value);

/*!
 * @brief Threads Interface
 *
 * The threads interface allows you to specify the threads there were running at the time an event happened. These threads can also contain stack traces. As per policy the thread that actually crashed with an exception should not have a stack trace but instead the thread_id attribute should be set on the exception and Sentry will connect the two.
 */
class thread_t
{
  public:
    /// running threads
    std::vector<thread_value_t> values;
};

void to_json(nlohmann::json& j, const thread_t& thread);

} // namespace interfaces
} // namespace types
} // namespace crow
