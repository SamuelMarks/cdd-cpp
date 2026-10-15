/// @file parse.hpp
/// @brief Google Discovery document parser declarations.

#pragma once
#include "../openapi/models.hpp"
#include "../utils/error.hpp"
#include <string>
#include <vector>

/// @namespace cdd_cpp::google_discovery
/// @brief Parser converting Google Discovery JSON documents into OpenAPI 3.2.0
/// specs.
namespace cdd_cpp::google_discovery {

/// @brief Standard Result alias for google_discovery operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse Google Discovery JSON and emit one or more OpenAPI objects.
/// @param input The raw Google Discovery JSON document or directory list JSON.
/// @return Result containing a vector of OpenAPI specifications or a typed
/// Error.
Result<std::vector<openapi::OpenAPI>> parse(const std::string &input) noexcept;

} // namespace cdd_cpp::google_discovery
