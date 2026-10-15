/// @file cpp_ast.hpp
/// @brief C++ Abstract Syntax Tree (AST) structures and parser/emitter
/// declarations.

#pragma once
#include <optional>
#include <string>
#include <vector>

namespace cdd_cpp::utils {

/// @brief Leading and trailing whitespace/comment trivia attached to an AST
/// node.
struct Trivia {
  std::string leading =
      {}; ///< Whitespace and non-doc comments preceding the node.
  std::string trailing = {}; ///< Whitespace and comments following the node.
};

/// @brief AST representation of a C++ struct/class field or function parameter.
struct CppField {
  std::string type =
      {}; ///< C++ type expression (including templates/qualifiers).
  std::string name = {}; ///< Identifier name of the field or parameter.
  std::string docstring =
      {};             ///< Doxygen or inline comment attached to the field.
  Trivia trivia = {}; ///< Formatting trivia surrounding the field.
};

/// @brief AST representation of a C++ free function or member method.
struct CppFunction {
  std::string return_type = {}; ///< Return type expression of the function.
  std::string name = {};        ///< Function or method identifier name.
  std::vector<CppField> params = {}; ///< Ordered list of function parameters.
  std::string body =
      {}; ///< Raw source body inside `{ ... }` (empty for declarations).
  std::string docstring = {}; ///< Preceding Doxygen comment block.
  Trivia trivia = {};         ///< Formatting trivia surrounding the function.
};

/// @brief AST representation of a C++ struct or class declaration.
struct CppClass {
  std::string name = {}; ///< Name of the struct or class.
  std::vector<CppField> fields =
      {}; ///< Member fields declared inside the class/struct.
  std::vector<CppFunction> methods =
      {}; ///< Member functions declared inside the class/struct.
  std::string docstring = {}; ///< Preceding Doxygen comment block.
  Trivia trivia = {}; ///< Formatting trivia surrounding the class/struct.
};

/// @brief Top-level C++ translation unit AST containing parsed classes and
/// functions.
struct CppAST {
  std::vector<CppClass> classes = {}; ///< Parsed struct and class definitions.
  std::vector<CppFunction> functions =
      {};                  ///< Parsed free and member functions.
  Trivia file_trivia = {}; ///< File-level leading and trailing trivia.
};

/// @brief Parse a C++ source string into a structured CppAST.
/// @param source The C++ header or implementation source code.
/// @return Populated CppAST containing classes, fields, functions, docstrings,
/// and trivia.
CppAST parse_cpp(const std::string &source) noexcept;

/// @brief Emit a CppAST back into formatted C++ source code.
/// @param ast The CppAST to serialize.
/// @return Generated C++ source string preserving trivia and docstrings.
std::string emit_cpp(const CppAST &ast) noexcept;

} // namespace cdd_cpp::utils
