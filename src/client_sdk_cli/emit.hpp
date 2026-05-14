#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

namespace cdd_cpp::client_sdk_cli {
/// Emit a C++ CLI application based on the OpenAPI spec
std::map<std::string, std::string> emit_cli(const openapi::OpenAPI &spec, bool no_github_actions = false, bool no_installable_package = false, bool tests = false) noexcept;
} // namespace cdd_cpp::client_sdk_cli
