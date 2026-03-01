#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::mocks {
/// Emit mocks to string
std::string emit(const openapi::OpenAPI &spec);
} // namespace cdd_cpp::mocks
