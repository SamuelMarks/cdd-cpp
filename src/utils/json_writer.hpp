/// @file json_writer.hpp
/// @brief Lightweight RFC 8259 JSON serializer and C++ identifier utilities.

#pragma once
#include <iomanip>
#include <map>
#include <optional>
#include <set>
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

/// @brief Check if an identifier is a reserved C++ keyword.
/// @param ident The identifier to check.
/// @return True if ident is a C++ reserved keyword.
inline bool is_cpp_keyword(std::string_view ident) noexcept {
  static const std::set<std::string_view> keywords = {
      "alignas",       "alignof",     "and",
      "and_eq",        "asm",         "auto",
      "bitand",        "bitor",       "bool",
      "break",         "case",        "catch",
      "char",          "char8_t",     "char16_t",
      "char32_t",      "class",       "compl",
      "concept",       "const",       "consteval",
      "constexpr",     "constinit",   "const_cast",
      "continue",      "co_await",    "co_return",
      "co_yield",      "decltype",    "default",
      "delete",        "do",          "double",
      "dynamic_cast",  "else",        "enum",
      "explicit",      "export",      "extern",
      "false",         "float",       "for",
      "friend",        "goto",        "if",
      "inline",        "int",         "long",
      "mutable",       "namespace",   "new",
      "noexcept",      "not",         "not_eq",
      "nullptr",       "operator",    "or",
      "or_eq",         "private",     "protected",
      "public",        "register",    "reinterpret_cast",
      "requires",      "return",      "short",
      "signed",        "sizeof",      "static",
      "static_assert", "static_cast", "struct",
      "switch",        "template",    "this",
      "thread_local",  "throw",       "true",
      "try",           "typedef",     "typeid",
      "typename",      "union",       "unsigned",
      "using",         "virtual",     "void",
      "volatile",      "wchar_t",     "while",
      "xor",           "xor_eq"};
  return keywords.contains(ident);
}

/// @brief Sanitize a property or parameter name so it is a valid C++
/// identifier.
/// @param name The raw property or parameter name.
/// @return Sanitized C++ identifier (with trailing '_' if it was a reserved
/// keyword).
inline std::string sanitize_cpp_identifier(std::string_view name) noexcept {
  std::string out{name};
  if (is_cpp_keyword(out)) {
    out += "_";
  }
  return out;
}

/// @brief Unsanitize a C++ field name back to its original OpenAPI property
/// name if it was a sanitized keyword.
/// @param name The C++ field identifier.
/// @return Original OpenAPI property name.
inline std::string unsanitize_cpp_identifier(std::string_view name) noexcept {
  if (name.size() > 1 && name.back() == '_') {
    std::string_view stem = name.substr(0, name.size() - 1);
    if (is_cpp_keyword(stem)) {
      return std::string(stem);
    }
  }
  return std::string(name);
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
