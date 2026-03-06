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
  void escape_string(const std::string &str) noexcept {
    ss << '"';
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
        if (static_cast<unsigned char>(c) <= 0x1f) {
          ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c
             << std::dec;
        } else {
          ss << c;
        }
      }
    }
    ss << '"';
  }

public:
  /// @brief Auto-generated doc.
  void start_object() noexcept {
    if (!first)
      ss << ",";
    ss << "{";
    first = true;
  }

  /// @brief Auto-generated doc.
  void end_object() noexcept {
    ss << "}";
    first = false;
  }

  /// @brief Auto-generated doc.
  void start_array() noexcept {
    if (!first)
      ss << ",";
    ss << "[";
    first = true;
  }

  /// @brief Auto-generated doc.
  void end_array() noexcept {
    ss << "]";
    first = false;
  }

  /// @brief Auto-generated doc.
  void key(const std::string &k) noexcept {
    if (!first) {
      ss << ",";
    }
    escape_string(k);
    ss << ":";
    first = true;
  }

  /// @brief Auto-generated doc.
  void raw_value(const std::string &v) noexcept {
    if (!first) {
      ss << ",";
    }
    ss << v;
    first = false;
  }

  /// @brief Auto-generated doc.
  void null_value() noexcept {
    ss << "null";
    first = false;
  }
  /// @brief Write string value
  void value(const std::string &v) noexcept {
    escape_string(v);
    first = false;
  }

  /// @brief Auto-generated doc.
  void value(const char *v) noexcept { value(std::string(v)); }

  /// @brief Auto-generated doc.
  void value(bool v) noexcept {
    ss << (v ? "true" : "false");
    first = false;
  }

  /// @brief Auto-generated doc.
  void value(int v) noexcept {
    ss << v;
    first = false;
  }

  /// @brief Auto-generated doc.
  void value(double v) noexcept {
    ss << v;
    first = false;
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k, const std::string &v) noexcept {
    key(k);
    value(v);
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k, bool v) noexcept {
    key(k);
    value(v);
  }

  /// @brief Auto-generated doc.
  void key_value(const std::string &k, const char *v) noexcept {
    key(k);
    value(std::string(v));
  }

  template <typename T>
  /// @brief Auto-generated doc.
  void key_optional(const std::string &k, const std::optional<T> &v) noexcept {
    if (v.has_value()) {
      key(k);
      value(v.value());
    }
  }

  std::string str() const noexcept { return ss.str(); }
};
} // namespace cdd_cpp::utils
