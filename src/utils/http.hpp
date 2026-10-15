/// @file http.hpp
/// @brief HTTP client utilities and global libcurl lifecycle management.

#pragma once
#include "error.hpp"
#include <string>

namespace cdd_cpp::utils {

/// @brief Process-wide RAII singleton for thread-safe libcurl global
/// initialization.
class CurlGlobalInitializer {
public:
  /// @brief Initialize libcurl global state once per process.
  CurlGlobalInitializer() noexcept;

  /// @brief Clean up libcurl global state on process exit.
  ~CurlGlobalInitializer() noexcept;

  CurlGlobalInitializer(const CurlGlobalInitializer &) = delete;
  CurlGlobalInitializer &operator=(const CurlGlobalInitializer &) = delete;

  /// @brief Access the process-wide singleton instance.
  /// @return Reference to the initialized CurlGlobalInitializer singleton.
  static const CurlGlobalInitializer &ensure_initialized() noexcept;
};

/// @brief Perform an HTTP GET request using libcurl.
/// @param url The target HTTP/HTTPS URL to fetch.
/// @return Result containing the response body string or a typed Error.
Result<std::string> http_get(const std::string &url) noexcept;

} // namespace cdd_cpp::utils
