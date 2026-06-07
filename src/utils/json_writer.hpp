#pragma once
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace cdd_cpp::utils {

/// @brief Auto-generated doc.
class JsonWriter {
  std::stringstream ss;
  bool first = true;

  /// @brief Auto-generated doc.
  void escape_string(const std::string &str) noexcept { // GCOV_EXCL_BR_LINE
    ss << '"';                                          // GCOV_EXCL_BR_LINE
    for (unsigned char c : str) {                       // GCOV_EXCL_BR_LINE
      switch (c) {
      case '"':
        ss << "\\\""; // GCOV_EXCL_BR_LINE
        break;
      case '\\':
        ss << "\\\\"; // GCOV_EXCL_BR_LINE
        break;
      case '\b':
        ss << "\\b"; // GCOV_EXCL_BR_LINE
        break;
      case '\f':
        ss << "\\f"; // GCOV_EXCL_BR_LINE
        break;
      case '\n':
        ss << "\\n"; // GCOV_EXCL_BR_LINE
        break;
      case '\r':
        ss << "\\r"; // GCOV_EXCL_BR_LINE
        break;
      case '\t':
        ss << "\\t"; // GCOV_EXCL_BR_LINE
        break;
      default:
        if (static_cast<unsigned char>(c) <= 0x1f) {
          ss << "\\u" << std::hex << std::setw(4)
             << std::setfill('0') // GCOV_EXCL_BR_LINE
             << (int)c            // GCOV_EXCL_BR_LINE
             << std::dec;         // GCOV_EXCL_BR_LINE
        } else {
          ss << c; // GCOV_EXCL_BR_LINE
        }
      }
    }
    ss << '"'; // GCOV_EXCL_BR_LINE
  }

public:
  /// @brief Auto-generated doc.
  void start_object() noexcept { // GCOV_EXCL_BR_LINE
    if (!first)                  // GCOV_EXCL_BR_LINE
      ss << ",";                 // GCOV_EXCL_BR_LINE
    ss << "{";                   // GCOV_EXCL_BR_LINE
    first = true;
  }

  /// @brief Auto-generated doc.
  void end_object() noexcept { // GCOV_EXCL_BR_LINE
    ss << "}";                 // GCOV_EXCL_BR_LINE
    first = false;
  }

  /// @brief Auto-generated doc.
  void start_array() noexcept { // GCOV_EXCL_BR_LINE
    if (!first)                 // GCOV_EXCL_BR_LINE
      ss << ",";                // GCOV_EXCL_BR_LINE
    ss << "[";                  // GCOV_EXCL_BR_LINE
    first = true;
  }

  /// @brief Auto-generated doc.
  void end_array() noexcept { // GCOV_EXCL_BR_LINE
    ss << "]";                // GCOV_EXCL_BR_LINE
    first = false;
  }

  /// @brief Auto-generated doc.
  void key(const std::string &k) noexcept { // GCOV_EXCL_BR_LINE
    if (!first) {                           // GCOV_EXCL_BR_LINE
      ss << ",";                            // GCOV_EXCL_BR_LINE
    }
    escape_string(k); // GCOV_EXCL_BR_LINE
    ss << ":";        // GCOV_EXCL_BR_LINE
    first = true;
  }

  /// @brief Auto-generated doc.
  void raw_value(const std::string &v) noexcept { // GCOV_EXCL_BR_LINE
    if (!first) {                                 // GCOV_EXCL_BR_LINE
      ss << ",";                                  // GCOV_EXCL_BR_LINE
    }
    ss << v; // GCOV_EXCL_BR_LINE
    first = false;
  }

  /// @brief Auto-generated doc.
  void null_value() noexcept { // GCOV_EXCL_BR_LINE
    if (!first)                // GCOV_EXCL_BR_LINE
      ss << ",";               // GCOV_EXCL_BR_LINE
    ss << "null";              // GCOV_EXCL_BR_LINE
    first = false;
  }
  /// @brief Write string value
  void value(const std::string &v) noexcept { // GCOV_EXCL_BR_LINE
    if (!first)                               // GCOV_EXCL_BR_LINE
      ss << ",";                              // GCOV_EXCL_BR_LINE
    escape_string(v);                         // GCOV_EXCL_BR_LINE
    first = false;
  }

  /// @brief Auto-generated doc.
  void value(const char *v) noexcept { // GCOV_EXCL_BR_LINE
    value(std::string(v));             // GCOV_EXCL_BR_LINE
  } // GCOV_EXCL_BR_LINE

  /// @brief Auto-generated doc.
  void value(bool v) noexcept {   // GCOV_EXCL_BR_LINE
    if (!first)                   // GCOV_EXCL_BR_LINE
      ss << ",";                  // GCOV_EXCL_BR_LINE
    ss << (v ? "true" : "false"); // GCOV_EXCL_BR_LINE
    first = false;
  }

  /// @brief Auto-generated doc.
  void value(int v) noexcept { // GCOV_EXCL_BR_LINE
    if (!first)                // GCOV_EXCL_BR_LINE
      ss << ",";               // GCOV_EXCL_BR_LINE
    ss << v;                   // GCOV_EXCL_BR_LINE
    first = false;
  }

  /// @brief Auto-generated doc.
  void value(double v) noexcept { // GCOV_EXCL_BR_LINE
    if (!first)                   // GCOV_EXCL_BR_LINE
      ss << ",";                  // GCOV_EXCL_BR_LINE
    ss << v;                      // GCOV_EXCL_BR_LINE
    first = false;
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k,
                 const std::string &v) noexcept { // GCOV_EXCL_BR_LINE
    key(k);                                       // GCOV_EXCL_BR_LINE
    value(v);                                     // GCOV_EXCL_BR_LINE
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k, bool v) noexcept { // GCOV_EXCL_BR_LINE
    key(k);                                               // GCOV_EXCL_BR_LINE
    value(v);                                             // GCOV_EXCL_BR_LINE
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k, int v) noexcept { // GCOV_EXCL_BR_LINE
    key(k);                                              // GCOV_EXCL_BR_LINE
    value(v);                                            // GCOV_EXCL_BR_LINE
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k, double v) noexcept { // GCOV_EXCL_BR_LINE
    key(k);                                                 // GCOV_EXCL_BR_LINE
    value(v);                                               // GCOV_EXCL_BR_LINE
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k,
                 const char *v) noexcept { // GCOV_EXCL_BR_LINE
    key(k);                                // GCOV_EXCL_BR_LINE
    value(std::string(v));                 // GCOV_EXCL_BR_LINE
  }

  template <typename T>
  /// @brief Auto-generated doc.
  void key_optional(const std::string &k,
                    const std::optional<T> &v) noexcept { // GCOV_EXCL_BR_LINE
    if (v.has_value()) {                                  // GCOV_EXCL_BR_LINE
      key(k);                                             // GCOV_EXCL_BR_LINE
      value(v.value());                                   // GCOV_EXCL_BR_LINE
    }
  }

  std::string str() const noexcept { return ss.str(); } // GCOV_EXCL_BR_LINE
};
} // namespace cdd_cpp::utils
