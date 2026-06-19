#pragma once
#include "models.hpp"
#include <string>

namespace cdd_cpp::openapi {
/// Emit OpenAPI to string
std::string emit(const OpenAPI &openapi) noexcept;
} // namespace cdd_cpp::openapi
