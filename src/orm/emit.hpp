#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::orm {
/// Emit C++ ORM code (using libpq) from OpenAPI spec
std::string emit(const openapi::OpenAPI &spec) noexcept;
} // namespace cdd_cpp::orm
