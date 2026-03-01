#pragma once
#include <string>

namespace cdd_cpp::utils {
/// Perform an HTTP GET request using curl
std::string http_get(const std::string &url);
} // namespace cdd_cpp::utils
