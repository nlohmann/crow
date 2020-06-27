#pragma once

#include <string>
#include <vector>
#include <crow/types/helpers/helpers.hpp>

namespace crow {
namespace types {
namespace interfaces {

class template_t
{
  public:
    /// the line number of the call
    int lineno{-1};

    /// the absolute path to the template on the file system
    std::string abs_path;

    /// the filename as it was passed to the template loader
    std::string filename;

    /// source code in filename at lineno
    std::string context_line;

    /// a list of source code lines before context_line (in order)
    std::vector<std::string> pre_context;

    /// a list of source code lines after context_line (in order)
    std::vector<std::string> post_context;
};

void to_json(nlohmann::json& j, const template_t& templ);

} // namespace interfaces
} // namespace types
} // namespace crow
