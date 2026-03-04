#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::classes {
/// Emit C++ models (structs) to string from OpenAPI spec
std::string emit(const openapi::OpenAPI &spec) noexcept;
} // namespace cdd_cpp::classes
