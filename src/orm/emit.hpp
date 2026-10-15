/// @file emit.hpp
/// @brief C++ ORM (libpq / Stub DAO) code generator declarations.

#pragma once
#include "../openapi/models.hpp"
#include <map>
#include <string>

/// @namespace cdd_cpp::orm
/// @brief Object-Relational Mapping (ORM) and DAO code generation for OpenAPI
/// schemas.
namespace cdd_cpp::orm {

/// @brief Emit C++ ORM code (using libpq) to a single string from an OpenAPI
/// spec.
/// @param spec The OpenAPI specification model.
/// @return Generated SQL DDL and C++ ORM class definitions.
std::string emit(const openapi::OpenAPI &spec) noexcept;

/// @brief Emit modular C++ ORM DAOs, Postgres implementations, Stubs, and
/// RepositoryFactory.
/// @param spec The OpenAPI specification model.
/// @param with_postgres If true, generates `Postgres<Model>Dao` classes using
/// libpq.
/// @param with_faker If true, populates `Stub<Model>Dao` responses with Faker
/// calls.
/// @return Map of relative filenames (`src/orm/...`, `migrations/init.sql`) to
/// file contents.
std::map<std::string, std::string> emit_modular(const openapi::OpenAPI &spec,
                                                bool with_postgres,
                                                bool with_faker) noexcept;

} // namespace cdd_cpp::orm
