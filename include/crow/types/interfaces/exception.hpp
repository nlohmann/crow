#pragma once

#include <string>
#include <crow/types/helpers/helpers.hpp>
#include <crow/types/interfaces/stacktrace.hpp>

namespace crow {
namespace types {
namespace interfaces {

/*!
 * @brief Mach Exception
 *
 * A Mach Exception on Apple systems comprising a code triple and optional descriptions.
 */
class mach_exception_t
{
  public:
    /// required numeric exception number
    int exception{-1};

    /// required numeric exception code
    int code{-1};

    /// required numeric exception subcode
    int subcode{-1};

    /// optional name of the exception constant in iOS / macOS
    std::string name;
};

void to_json(nlohmann::json& j, const mach_exception_t& mach_exception);

/*!
 * @brief Information on the POSIX signal
 *
 * On Apple systems, signals also carry a code in addition to the signal number describing the signal in more detail. On Linux, this code does not exist.
 */
class signal_t
{
  public:
    /// the POSIX signal number
    int number{-1};

    /// optional Apple signal code
    int code{-1};

    /// optional name of the signal based on the signal number
    std::string name;

    /// optional name of the signal code
    std::string code_name;
};

void to_json(nlohmann::json& j, const signal_t& signal);

/*!
 * Error codes set by linux system calls and some library functions as specified in ISO C99, POSIX.1-2001 and POSIX.1-2008. See errno(3) for more information.
 */
class errno_t
{
  public:
    /// the error number
    int number{-1};

    /// optional name of the error
    std::string name;
};

void to_json(nlohmann::json& j, const errno_t& errno_);

/*!
 * @brief Meta information
 *
 * The mechanism meta data usually carries error codes reported by the runtime or operating system, along with a platform dependent interpretation of these codes. SDKs can safely omit code names and descriptions for well known error codes, as it will be filled out by Sentry. For proprietary or vendor-specific error codes, adding these values will give additional information to the user.
 */
class meta_t
{
  public:
    /// information on the POSIX signal
    signal_t signal;

    /// a Mach Exception on Apple systems
    mach_exception_t mach_exception;

    /// error codes set by linux system calls
    errno_t errno_;
};

void to_json(nlohmann::json& j, const meta_t& meta);

/*!
 * @brief Exception Mechanism Interface
 *
 * The exception mechanism is an optional field residing in the Exception Interface. It carries additional information about the way the exception was created on the target system. This includes general exception values obtained from operating system or runtime APIs, as well as mechanism-specific values.
 */
class mechanism_t
{
  public:
    /// required unique identifier of this mechanism determining rendering and processing of the mechanism data
    std::string type;

    /// optional human readable description of the error mechanism and a possible hint on how to solve this error
    std::string description;

    /// optional fully qualified URL to an online help resource, possible interpolated with error parameters
    std::string help_link;

    /// optional flag indicating whether the exception has been handled by the user (e.g. via try..catch)
    optional_bool handled;

    /// optional information from the operating system or runtime on the exception mechanism
    meta_t meta;

    /// arbitrary extra data that might help the user understand the error thrown by this mechanism
    nlohmann::json data;
};

void to_json(nlohmann::json& j, const mechanism_t& mechanism);

/*!
 * @brief Exception Interface
 *
 * An exception consists of a list of values. In most cases, this list contains a single exception, with an optional stack trace interface. Multiple values represent a chained exception, and should be sent oldest to newest.
 */
class exception_t
{
  public:
    exception_t() = default;
    explicit exception_t(const std::exception& exception, bool handled = true);
    explicit exception_t(int signal, bool handled = true);

    /// the type of exception, e.g. ValueError
    std::string type{};

    /// the value of the exception
    std::string value{};

    /// the optional module, or package which the exception type lives in
    std::string module{};

    /// an optional value which refers to a thread in the threads interface
    std::size_t thread_id{};

    /// an optional object describing the mechanism that created this exception
    mechanism_t mechanism{};

    /// stack trace information
    stacktrace_t stacktrace{};
};

void to_json(nlohmann::json& j, const exception_t& exception);

} // namespace interfaces
} // namespace types
} // namespace crow
