// GCOV_EXCL_BR_START
#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::models {
/// Emit C++ models (structs) to string from OpenAPI spec
std::string emit(const openapi::OpenAPI &spec) noexcept;
} // namespace cdd_cpp::models

// GCOV_EXCL_BR_STOP
