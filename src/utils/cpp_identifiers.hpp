/// @file cpp_identifiers.hpp
/// @brief C++ keyword detection and identifier sanitization/unsanitization
/// utilities.

#pragma once
#include <set>
#include <string>
#include <string_view>

namespace cdd_cpp::utils {

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

} // namespace cdd_cpp::utils
