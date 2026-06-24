#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

namespace cdd_cpp::orm {
/// Emit C++ ORM code (using libpq) from OpenAPI spec (Legacy)
std::string emit(const openapi::OpenAPI &spec) noexcept;

/// Emit modular C++ ORM DAOs
std::map<std::string, std::string> emit_modular(const openapi::OpenAPI &spec,
                                                bool with_postgres,
                                                bool with_faker) noexcept;
} // namespace cdd_cpp::orm
