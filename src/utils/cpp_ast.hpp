#pragma once
#include <string>
#include <vector>
#include <optional>

namespace cdd_cpp::utils {

  /// @brief Auto-generated doc.
struct Trivia {
  std::string leading;
  std::string trailing;
};

  /// @brief Auto-generated doc.
struct CppField {
  std::string type;
  std::string name;
  std::string docstring;
  Trivia trivia;
};

  /// @brief Auto-generated doc.
struct CppClass {
  std::string name;
  std::string docstring;
  std::vector<CppField> fields;
  Trivia trivia;
};

  /// @brief Auto-generated doc.
struct CppFunction {
  std::string return_type;
  std::string name;
  std::vector<CppField> params;
  std::string docstring;
  std::string body;
  Trivia trivia;
};

  /// @brief Auto-generated doc.
struct CppAST {
  std::vector<CppClass> classes;
  std::vector<CppFunction> functions;
  Trivia file_trivia;
};

  /// @brief Auto-generated doc.
CppAST parse_cpp(const std::string &source);
  /// @brief Auto-generated doc.
std::string emit_cpp(const CppAST &ast);

} // namespace cdd_cpp::utils
