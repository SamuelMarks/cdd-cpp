#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

namespace cdd_cpp::client_sdk {
/// Emit a C++ client SDK containing libcurl and simdjson wrappers based on the
/// OpenAPI spec. Returns a map of filename -> content.
std::map<std::string, std::string>
emit_client(const openapi::OpenAPI &spec, bool no_github_actions = false,
            bool no_installable_package = false, bool tests = false) noexcept;
} // namespace cdd_cpp::client_sdk
