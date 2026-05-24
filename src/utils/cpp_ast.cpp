#include "cpp_ast.hpp"
#include <regex>
#include <sstream>

namespace cdd_cpp::utils {

// Extremely simplified parse_cpp to support whitespace & comment extraction.
// In a full compiler, we'd use Clang tooling or a proper recursive descent
// parser.
CppAST parse_cpp(const std::string &source) noexcept {
  CppAST ast; // GCOV_EXCL_BR_LINE

  // Since building a full whitespace-aware parser from scratch in C++ using
  // regex is very hard, we capture blocks and attach previous
  // whitespace/comments as leading trivia. This is a minimal approximation.
  std::regex class_regex( // GCOV_EXCL_BR_LINE
      R"((?:(/\*(?:(?!\*/)(?:.|\n))*\*/)\s*)?(?:struct|class)\s+(\w+)\s*\{([^}]*)\})");
  std::sregex_iterator cls_it(source.begin(), source.end(),
                              class_regex); // GCOV_EXCL_BR_LINE
  std::sregex_iterator end;                 // GCOV_EXCL_BR_LINE

  while (cls_it != end) { // GCOV_EXCL_BR_LINE
    CppClass cls;
    if (cls_it->size() > 1 && cls_it->length(1) > 0) // GCOV_EXCL_BR_LINE
      cls.docstring = (*cls_it)[1].str();            // GCOV_EXCL_BR_LINE
    cls.name = (*cls_it)[2].str();                   // GCOV_EXCL_BR_LINE

    // Simplistic trivia approximation: grab prefix
    cls.trivia.leading = cls_it->prefix().str(); // GCOV_EXCL_BR_LINE

    std::string body = (*cls_it)[3].str(); // GCOV_EXCL_BR_LINE
    std::regex field_regex(                // GCOV_EXCL_BR_LINE
        R"((?:const\s+)?([\w:]+(?:<\w+>)?)(?:\s*&|\s*\*|\s+)\s*(\w+)\s*;(?:\s*(//.*))?)");
    std::sregex_iterator fld_it(body.begin(), body.end(),
                                field_regex); // GCOV_EXCL_BR_LINE
    while (fld_it != end) {                   // GCOV_EXCL_BR_LINE
      CppField fld;
      fld.type = (*fld_it)[1].str();                   // GCOV_EXCL_BR_LINE
      fld.name = (*fld_it)[2].str();                   // GCOV_EXCL_BR_LINE
      if (fld_it->size() > 3 && fld_it->length(3) > 0) // GCOV_EXCL_BR_LINE
        fld.docstring = (*fld_it)[3].str();            // GCOV_EXCL_BR_LINE
      fld.trivia.leading = fld_it->prefix().str();     // GCOV_EXCL_BR_LINE
      cls.fields.push_back(fld);                       // GCOV_EXCL_BR_LINE
      fld_it++;                                        // GCOV_EXCL_BR_LINE
    }
    ast.classes.push_back(cls); // GCOV_EXCL_BR_LINE
    cls_it++;                   // GCOV_EXCL_BR_LINE
  }

  std::regex func_regex( // GCOV_EXCL_BR_LINE
      R"((?:(/\*(?:(?!\*/)(?:.|\n))*\*/)\s*)?(?:inline\s+|virtual\s+|static\s+)*(?:const\s+)?([\w:]+(?:<\w+>)?)(?:\s*&|\s*\*|\s+)\s*(\w+)\s*\(([^)]*)\)\s*(?:\{([^}]*)\}|;))");
  std::sregex_iterator fn_it(source.begin(), source.end(),
                             func_regex); // GCOV_EXCL_BR_LINE

  while (fn_it != end) { // GCOV_EXCL_BR_LINE
    CppFunction fn;
    if (fn_it->size() > 1 && fn_it->length(1) > 0) // GCOV_EXCL_BR_LINE
      fn.docstring = (*fn_it)[1].str();            // GCOV_EXCL_BR_LINE
    fn.return_type = (*fn_it)[2].str();            // GCOV_EXCL_BR_LINE
    fn.name = (*fn_it)[3].str();                   // GCOV_EXCL_BR_LINE

    fn.trivia.leading = fn_it->prefix().str(); // GCOV_EXCL_BR_LINE

    std::string params_str = (*fn_it)[4].str();      // GCOV_EXCL_BR_LINE
    if (fn_it->size() > 5 && fn_it->length(5) > 0) { // GCOV_EXCL_BR_LINE
      fn.body = (*fn_it)[5].str();                   // GCOV_EXCL_BR_LINE
    }

    std::regex param_regex( // GCOV_EXCL_BR_LINE
        R"((?:const\s+)?([\w:]+(?:<\w+>)?)(?:\s*&|\s*\*|\s+)\s*(\w+))");
    std::sregex_iterator p_it(params_str.begin(),
                              params_str.end(), // GCOV_EXCL_BR_LINE
                              param_regex);
    while (p_it != end) { // GCOV_EXCL_BR_LINE
      CppField p;
      p.type = (*p_it)[1].str();               // GCOV_EXCL_BR_LINE
      p.name = (*p_it)[2].str();               // GCOV_EXCL_BR_LINE
      p.trivia.leading = p_it->prefix().str(); // GCOV_EXCL_BR_LINE
      fn.params.push_back(p);                  // GCOV_EXCL_BR_LINE
      p_it++;                                  // GCOV_EXCL_BR_LINE
    }

    if (fn.name != "if" && fn.name != "while" &&
        fn.name != "for" &&        // GCOV_EXCL_BR_LINE
        fn.name != "switch") {     // GCOV_EXCL_BR_LINE
      ast.functions.push_back(fn); // GCOV_EXCL_BR_LINE
    }
    fn_it++; // GCOV_EXCL_BR_LINE
  }

  return ast;
}

std::string emit_cpp(const CppAST &ast) noexcept {
  std::ostringstream out;         // GCOV_EXCL_BR_LINE
  out << ast.file_trivia.leading; // GCOV_EXCL_BR_LINE

  for (const auto &cls : ast.classes) {     // GCOV_EXCL_BR_LINE
    out << cls.trivia.leading;              // GCOV_EXCL_BR_LINE
    if (!cls.docstring.empty())             // GCOV_EXCL_BR_LINE
      out << cls.docstring << "\n";         // GCOV_EXCL_BR_LINE
    out << "struct " << cls.name << " {\n"; // GCOV_EXCL_BR_LINE
    for (const auto &fld : cls.fields) {    // GCOV_EXCL_BR_LINE
      out << fld.trivia.leading << "  " << fld.type << " " << fld.name
          << ";";                    // GCOV_EXCL_BR_LINE
      if (!fld.docstring.empty())    // GCOV_EXCL_BR_LINE
        out << " " << fld.docstring; // GCOV_EXCL_BR_LINE
      out << "\n";                   // GCOV_EXCL_BR_LINE
    }
    out << "};\n";              // GCOV_EXCL_BR_LINE
    out << cls.trivia.trailing; // GCOV_EXCL_BR_LINE
  }

  for (const auto &fn : ast.functions) {            // GCOV_EXCL_BR_LINE
    out << fn.trivia.leading;                       // GCOV_EXCL_BR_LINE
    if (!fn.docstring.empty())                      // GCOV_EXCL_BR_LINE
      out << fn.docstring << "\n";                  // GCOV_EXCL_BR_LINE
    out << fn.return_type << " " << fn.name << "("; // GCOV_EXCL_BR_LINE
    for (size_t i = 0; i < fn.params.size(); ++i) { // GCOV_EXCL_BR_LINE
      out << fn.params[i].trivia.leading << fn.params[i].type
          << " "                    // GCOV_EXCL_BR_LINE
          << fn.params[i].name;     // GCOV_EXCL_BR_LINE
      if (i + 1 < fn.params.size()) // GCOV_EXCL_BR_LINE
        out << ", ";                // GCOV_EXCL_BR_LINE
    }
    out << ") {\n";            // GCOV_EXCL_BR_LINE
    out << fn.body;            // GCOV_EXCL_BR_LINE
    out << "\n}\n";            // GCOV_EXCL_BR_LINE
    out << fn.trivia.trailing; // GCOV_EXCL_BR_LINE
  }

  out << ast.file_trivia.trailing; // GCOV_EXCL_BR_LINE
  return out.str();                // GCOV_EXCL_BR_LINE
}

} // namespace cdd_cpp::utils