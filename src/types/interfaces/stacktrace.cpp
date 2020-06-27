#include <crow/types/interfaces/stacktrace.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const stacktrace_t& stacktrace)
{
    fill_if_not_empty("frames", stacktrace.frames, j);
    if (stacktrace.frames_omitted.first != -1 and stacktrace.frames_omitted.second != -1)
    {
        j["frames_omitted"] = stacktrace.frames_omitted;
    }
}

void to_json(nlohmann::json& j, const frame_t& frame)
{
    fill_if_not_empty("filename", frame.filename, j);
    fill_if_not_empty("function", frame.function, j);
    fill_if_not_empty("module", frame.module, j);
    fill_if_not_empty("lineno", frame.lineno, j);
    fill_if_not_empty("colno", frame.colno, j);
    fill_if_not_empty("abs_path", frame.abs_path, j);
    fill_if_not_empty("context_line", frame.context_line, j);
    fill_if_not_empty("pre_context", frame.pre_context, j);
    fill_if_not_empty("post_context", frame.post_context, j);
    fill_if_not_empty("in_app", frame.in_app, j);
    fill_if_not_empty("vars", frame.vars, j);
    fill_if_not_empty("package", frame.package, j);
    fill_if_not_empty("platform", frame.platform, j);
    fill_if_not_empty("image_addr", frame.image_addr, j);
    fill_if_not_empty("instruction_addr", frame.instruction_addr, j);
    fill_if_not_empty("symbol_addr", frame.symbol_addr, j);
    fill_if_not_empty("instruction_offset", frame.instruction_offset, j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
