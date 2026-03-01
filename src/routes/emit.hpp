#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::routes {
/// Emit C++ server routes to string
std::string emit(const openapi::OpenAPI &spec);
} // namespace cdd_cpp::routes
