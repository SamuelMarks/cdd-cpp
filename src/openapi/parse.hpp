#pragma once
#include "models.hpp"
#include <simdjson.h>
#include <string>

namespace cdd_cpp::openapi {
/// Parse OpenAPI from string
OpenAPI parse(const std::string &input);
} // namespace cdd_cpp::openapi
