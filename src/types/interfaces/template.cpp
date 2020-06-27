#include <crow/types/interfaces/template.hpp>

namespace crow {
namespace types {
namespace interfaces {

void to_json(nlohmann::json& j, const template_t& templ)
{
    fill_if_not_empty("lineno", templ.lineno, j);
    fill_if_not_empty("abs_path", templ.abs_path, j);
    fill_if_not_empty("filename", templ.filename, j);
    fill_if_not_empty("context_line", templ.context_line, j);
    fill_if_not_empty("pre_context", templ.pre_context, j);
    fill_if_not_empty("post_context", templ.post_context, j);
}

} // namespace interfaces
} // namespace types
} // namespace crow
