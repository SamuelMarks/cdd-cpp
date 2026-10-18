/// @file parse.hpp
/// @brief Arazzo Specification 1.1.0 JSON parser declarations.

#pragma once
#include "../utils/error.hpp"
#include "models.hpp"
#include <string>

namespace cdd_cpp::arazzo {

/// @brief Standard Result alias for Arazzo operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse an Arazzo 1.1.0 specification from a JSON string.
/// @param json_str The raw JSON string representing an Arazzo document.
/// @return Result containing the parsed Arazzo model or a typed Error.
Result<Arazzo> parse(const std::string &json_str) noexcept;

} // namespace cdd_cpp::arazzo
