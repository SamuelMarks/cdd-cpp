/// @file parse.hpp
/// @brief GoogleMock C++ mock client parser declarations.

#pragma once
#include "../openapi/models.hpp"
#include "../utils/error.hpp"
#include <string>

namespace cdd_cpp::mocks {

/// @brief Standard Result alias for mocks operations.
template <typename T> using Result = utils::Result<T>;

/// @brief Parse GoogleMock `MOCK_METHOD` and `IClient` declarations into an
/// OpenAPI specification.
/// @param input The C++ mock header source code to parse.
/// @return Result containing the extracted OpenAPI specification or a typed
/// Error.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept;

} // namespace cdd_cpp::mocks
