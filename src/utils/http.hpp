#pragma once
#include <expected>
#include <string>

namespace cdd_cpp::utils {
/// Perform an HTTP GET request using curl
std::expected<std::string, std::string>
http_get(const std::string &url) noexcept;
} // namespace cdd_cpp::utils
