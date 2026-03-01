#pragma once
#include "../openapi/models.hpp"
#include <string>

namespace cdd_cpp::classes {
/// Parse a C++ Client SDK and return an OpenAPI specification
openapi::OpenAPI parse(const std::string &input);
} // namespace cdd_cpp::classes
