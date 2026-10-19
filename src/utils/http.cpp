/// @file http.cpp
/// @brief Implementation of HTTP GET client and libcurl RAII initializer.

#include "http.hpp"

#if !defined(__wasi__) && !defined(__EMSCRIPTEN__)
#include <curl/curl.h>

namespace cdd_cpp::utils {

CurlGlobalInitializer::CurlGlobalInitializer() noexcept {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

CurlGlobalInitializer::~CurlGlobalInitializer() noexcept {
  curl_global_cleanup();
}

/// @brief Process-wide libcurl RAII singleton instance.
static const CurlGlobalInitializer global_curl_init;

const CurlGlobalInitializer &
CurlGlobalInitializer::ensure_initialized() noexcept {
  return global_curl_init;
}

/// @brief Libcurl write callback appending received bytes to a std::string.
/// @param contents Pointer to the received data buffer.
/// @param size Size of each data element.
/// @param nmemb Number of elements received.
/// @param userp User pointer to the destination std::string.
/// @return Total number of bytes processed.
static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) noexcept {
  static_cast<std::string *>(userp)->append(static_cast<char *>(contents),
                                            size * nmemb);
  return size * nmemb;
}

/// @brief Execute HTTP client operation via `http_get`.
Result<std::string> http_get(const std::string &url) noexcept {
  (void)CurlGlobalInitializer::ensure_initialized();
  CURL *curl = curl_easy_init();
  std::string readBuffer;

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK) {
    return make_error(ErrorCode::HttpPerformFailed,
                      "curl_easy_perform() failed", url);
  }
  return readBuffer;
}

} // namespace cdd_cpp::utils
#else
namespace cdd_cpp::utils {

CurlGlobalInitializer::CurlGlobalInitializer() noexcept {}

CurlGlobalInitializer::~CurlGlobalInitializer() noexcept {}

const CurlGlobalInitializer &
CurlGlobalInitializer::ensure_initialized() noexcept {
  static const CurlGlobalInitializer instance;
  return instance;
}

/// @brief Execute HTTP client operation via `http_get`.
Result<std::string> http_get(const std::string &url) noexcept {
  (void)CurlGlobalInitializer::ensure_initialized();
  return make_error(ErrorCode::WasiUnsupportedOperation,
                    "http_get() not implemented in WASI", url);
}

} // namespace cdd_cpp::utils
#endif
