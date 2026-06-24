#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

namespace cdd_cpp::models {
/// Emit C++ models (structs) to string from OpenAPI spec (Legacy, for client
/// SDKs)
std::string emit(const openapi::OpenAPI &spec) noexcept;

/// Emit modular C++ models (structs) to map of files
std::map<std::string, std::string>
emit_modular(const openapi::OpenAPI &spec) noexcept;
} // namespace cdd_cpp::models
