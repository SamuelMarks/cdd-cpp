#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::classes {
/// Emit a C++ CLI application based on the OpenAPI spec
std::string emit_cli(const openapi::OpenAPI &spec);
} // namespace cdd_cpp::classes
