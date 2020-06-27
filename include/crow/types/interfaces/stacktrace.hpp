#pragma once

#include <string>
#include <utility>
#include <vector>
#include <crow/types/helpers/base_types.hpp>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

class frame_t
{
  public:
    /// the relative filepath to the call
    std::string filename;

    /// the name of the function being called
    std::string function;

    /// platform-specific module path
    std::string module;

    /// the line number of the call
    int lineno{-1};

    /// the column number of the call
    int colno{-1};

    /// the absolute path to filename
    std::string abs_path;

    /// source code in filename at lineno
    std::string context_line;

    /// a list of source code lines before context_line (in order)
    std::vector<std::string> pre_context;

    /// a list of source code lines after context_line (in order)
    std::vector<std::string> post_context;

    /// signifies whether this frame is related to the execution of the relevant code in this stack trace
    optional_bool in_app;

    /// a mapping of variables which were available within this frame (usually context-locals)
    nlohmann::json vars;

    /// the "package" the frame was contained in
    std::string package;

    /// this can override the platform for a single frame
    std::string platform;

    /// optionally an address of the debug image to reference
    std::string image_addr;

    /// an optional instruction address for symbolication
    std::string instruction_addr;

    /// an optional address that points to a symbol
    std::string symbol_addr;

    /// the difference between instruction address and symbol address in bytes
    int instruction_offset{-1};
};

void to_json(nlohmann::json& j, const frame_t& frame);

class stacktrace_t
{
  public:
    /// list of frames
    std::vector<frame_t> frames;

    /// which frames have been omitted (start, end)
    std::pair<int, int> frames_omitted{-1, -1};
};

void to_json(nlohmann::json& j, const stacktrace_t& stacktrace);

} // namespace interfaces
} // namespace types
} // namespace crow
