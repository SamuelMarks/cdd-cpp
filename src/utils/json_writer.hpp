/// @file json_writer.hpp
/// @brief Lightweight RFC 8259 JSON serializer and C++ identifier utilities.

#pragma once
#include "cpp_identifiers.hpp"
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace cdd_cpp::utils {

/// @brief Escape a raw string into RFC 8259 JSON string content (without
/// surrounding quotes).
/// @param str The raw input string to escape.
/// @return Escaped JSON string content.
inline std::string escape_json_string(std::string_view str) noexcept {
  std::ostringstream ss;
  for (unsigned char c : str) {
    switch (c) {
    case '"':
      ss << "\\\"";
      break;
    case '\\':
      ss << "\\\\";
      break;
    case '\b':
      ss << "\\b";
      break;
    case '\f':
      ss << "\\f";
      break;
    case '\n':
      ss << "\\n";
      break;
    case '\r':
      ss << "\\r";
      break;
    case '\t':
      ss << "\\t";
      break;
    default:
      if (c <= 0x1f) {
        ss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
           << static_cast<int>(c) << std::dec;
      } else {
        ss << static_cast<char>(c);
      }
    }
  }
  return ss.str();
}

/// @brief Streaming JSON writer for emitting compact RFC 8259 JSON documents.
class JsonWriter {
  std::stringstream ss; ///< Internal string buffer accumulating JSON output.
  bool first =
      true; ///< True if the next element in the current container is the first.

  /// @brief Write a quoted and escaped JSON string to the internal buffer.
  /// @param str The raw string to quote and escape.
  void escape_string(const std::string &str) noexcept {
    ss << '"' << escape_json_string(str) << '"';
  }

public:
  /// @brief Begin a new JSON object ('{').
  void start_object() noexcept {
    if (!first)
      ss << ",";
    ss << "{";
    first = true;
  }

  /// @brief End the current JSON object ('}').
  void end_object() noexcept {
    ss << "}";
    first = false;
  }

  /// @brief Begin a new JSON array ('[').
  void start_array() noexcept {
    if (!first)
      ss << ",";
    ss << "[";
    first = true;
  }

  /// @brief End the current JSON array (']').
  void end_array() noexcept {
    ss << "]";
    first = false;
  }

  /// @brief Write an object property key followed by a colon.
  /// @param k The property key name.
  void key(const std::string &k) noexcept {
    if (!first) {
      ss << ",";
    }
    escape_string(k);
    ss << ":";
    first = true;
  }

  /// @brief Write a pre-serialized raw JSON fragment as a value.
  /// @param v The raw JSON value string.
  void raw_value(const std::string &v) noexcept {
    if (!first) {
      ss << ",";
    }
    ss << v;
    first = false;
  }

  /// @brief Write a JSON null literal.
  void null_value() noexcept {
    if (!first)
      ss << ",";
    ss << "null";
    first = false;
  }

  /// @brief Write a quoted and escaped JSON string value.
  /// @param v The string value to write.
  void value(const std::string &v) noexcept {
    if (!first)
      ss << ",";
    escape_string(v);
    first = false;
  }

  /// @brief Write a C-string value as a quoted and escaped JSON string.
  /// @param v Null-terminated C-string to write.
  void value(const char *v) noexcept { value(std::string(v)); }

  /// @brief Write a boolean literal ('true' or 'false').
  /// @param v The boolean value to write.
  void value(bool v) noexcept {
    if (!first)
      ss << ",";
    ss << (v ? "true" : "false");
    first = false;
  }

  /// @brief Write an integer value.
  /// @param v The integer value to write.
  void value(int v) noexcept {
    if (!first)
      ss << ",";
    ss << v;
    first = false;
  }

  /// @brief Write a floating-point number value.
  /// @param v The double value to write.
  void value(double v) noexcept {
    if (!first)
      ss << ",";
    ss << v;
    first = false;
  }

  /// @brief Write an object key and string value pair.
  /// @param k The property key.
  /// @param v The string value.
  void key_value(const std::string &k, const std::string &v) noexcept {
    key(k);
    value(v);
  }

  /// @brief Write an object key and boolean value pair.
  /// @param k The property key.
  /// @param v The boolean value.
  void key_value(const std::string &k, bool v) noexcept {
    key(k);
    value(v);
  }

  /// @brief Write an object key and integer value pair.
  /// @param k The property key.
  /// @param v The integer value.
  void key_value(const std::string &k, int v) noexcept {
    key(k);
    value(v);
  }

  /// @brief Write an object key and double value pair.
  /// @param k The property key.
  /// @param v The double value.
  void key_value(const std::string &k, double v) noexcept {
    key(k);
    value(v);
  }

  /// @brief Write an object key and C-string value pair.
  /// @param k The property key.
  /// @param v The C-string value.
  void key_value(const std::string &k, const char *v) noexcept {
    key(k);
    value(std::string(v));
  }

  /// @brief Write an object key and optional value if present.
  /// @tparam T The underlying value type inside std::optional.
  /// @param k The property key.
  /// @param v The optional value.
  template <typename T>
  void key_optional(const std::string &k, const std::optional<T> &v) noexcept {
    if (v.has_value()) {
      key(k);
      value(*v);
    }
  }

  /// @brief Return the serialized JSON string.
  /// @return The complete JSON string accumulated so far.
  std::string str() const noexcept { return ss.str(); }
};

} // namespace cdd_cpp::utils
