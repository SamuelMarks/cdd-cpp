#include "cpp_ast.hpp"
#include <regex>
#include <sstream>

namespace cdd_cpp::utils {

// Extremely simplified parse_cpp to support whitespace & comment extraction.
// In a full compiler, we'd use Clang tooling or a proper recursive descent
// parser.
CppAST parse_cpp(const std::string &source) {
  CppAST ast;

  // Since building a full whitespace-aware parser from scratch in C++ using
  // regex is very hard, we capture blocks and attach previous
  // whitespace/comments as leading trivia. This is a minimal approximation.
  std::regex class_regex(
      R"((?:(/\*(?:.|\n)*?\*/)\s*)?(?:struct|class)\s+(\w+)\s*\{([^}]*)\})");
  std::sregex_iterator cls_it(source.begin(), source.end(), class_regex);
  std::sregex_iterator end;

  while (cls_it != end) {
    CppClass cls;
    if (cls_it->size() > 1 && cls_it->length(1) > 0)
      cls.docstring = (*cls_it)[1].str();
    cls.name = (*cls_it)[2].str();

    // Simplistic trivia approximation: grab prefix
    cls.trivia.leading = cls_it->prefix().str();

    std::string body = (*cls_it)[3].str();
    std::regex field_regex(
        R"((\w+(?:::\w+)*(?:<\w+>)?)\s+(\w+)\s*;(?:\s*(//.*))?)");
    std::sregex_iterator fld_it(body.begin(), body.end(), field_regex);
    while (fld_it != end) {
      CppField fld;
      fld.type = (*fld_it)[1].str();
      fld.name = (*fld_it)[2].str();
      if (fld_it->size() > 3 && fld_it->length(3) > 0)
        fld.docstring = (*fld_it)[3].str();
      fld.trivia.leading = fld_it->prefix().str();
      cls.fields.push_back(fld);
      fld_it++;
    }
    ast.classes.push_back(cls);
    cls_it++;
  }

  std::regex func_regex(
      R"((?:(/\*(?:.|\n)*?\*/)\s*)?(?:inline\s+|virtual\s+|static\s+)*(\w+(?:::\w+)*(?:<\w+>)?)\s+(\w+)\s*\(([^)]*)\)\s*(?:\{([^}]*)\}|;))");
  std::sregex_iterator fn_it(source.begin(), source.end(), func_regex);

  while (fn_it != end) {
    CppFunction fn;
    if (fn_it->size() > 1 && fn_it->length(1) > 0)
      fn.docstring = (*fn_it)[1].str();
    fn.return_type = (*fn_it)[2].str();
    fn.name = (*fn_it)[3].str();

    fn.trivia.leading = fn_it->prefix().str();

    std::string params_str = (*fn_it)[4].str();
    if (fn_it->size() > 5 && fn_it->length(5) > 0) {
      fn.body = (*fn_it)[5].str();
    }

    std::regex param_regex(R"((\w+(?:::\w+)*(?:<\w+>)?)\s+(\w+))");
    std::sregex_iterator p_it(params_str.begin(), params_str.end(),
                              param_regex);
    while (p_it != end) {
      CppField p;
      p.type = (*p_it)[1].str();
      p.name = (*p_it)[2].str();
      p.trivia.leading = p_it->prefix().str();
      fn.params.push_back(p);
      p_it++;
    }

    if (fn.name != "if" && fn.name != "while" && fn.name != "for" &&
        fn.name != "switch") {
      ast.functions.push_back(fn);
    }
    fn_it++;
  }

  return ast;
}

std::string emit_cpp(const CppAST &ast) {
  std::ostringstream out;
  out << ast.file_trivia.leading;

  for (const auto &cls : ast.classes) {
    out << cls.trivia.leading;
    if (!cls.docstring.empty())
      out << cls.docstring << "\n";
    out << "struct " << cls.name << " {\n";
    for (const auto &fld : cls.fields) {
      out << fld.trivia.leading << "  " << fld.type << " " << fld.name << ";";
      if (!fld.docstring.empty())
        out << " " << fld.docstring;
      out << "\n";
    }
    out << "};\n";
    out << cls.trivia.trailing;
  }

  for (const auto &fn : ast.functions) {
    out << fn.trivia.leading;
    if (!fn.docstring.empty())
      out << fn.docstring << "\n";
    out << fn.return_type << " " << fn.name << "(";
    for (size_t i = 0; i < fn.params.size(); ++i) {
      out << fn.params[i].trivia.leading << fn.params[i].type << " "
          << fn.params[i].name;
      if (i + 1 < fn.params.size())
        out << ", ";
    }
    out << ") {\n";
    out << fn.body;
    out << "\n}\n";
    out << fn.trivia.trailing;
  }

  out << ast.file_trivia.trailing;
  return out.str();
}

} // namespace cdd_cpp::utils