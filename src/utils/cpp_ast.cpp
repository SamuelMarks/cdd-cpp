/// @file cpp_ast.cpp
/// @brief Recursive-descent C++ declaration/body parser and emitter supporting
/// nested braces, templates, and `///` / `/* */` docstrings.

#include "cpp_ast.hpp"
#include <cctype>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>

namespace cdd_cpp::utils {

/// @brief Reserved C++ declaration keywords to ignore when parsing class
/// fields.
static const std::set<std::string_view> kSkipFieldTypes{
    "return", "using", "typedef", "friend", "class", "struct"};

/// @brief Check whether a character is a valid C++ identifier character.
/// @param c Character to inspect.
/// @return True if alphanumeric or `_`.
static bool is_ident_char(char c) noexcept {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

/// @brief Find the matching closing delimiter (`}`, `)`, `>`) respecting
/// strings, chars, and comments.
/// @param src The source string to scan.
/// @param open_pos Index of the opening delimiter (`{`, `(`, `<`).
/// @param open_ch Opening character.
/// @param close_ch Closing character.
/// @return Index of the matching closing character, or std::string::npos if
/// unbalanced.
static size_t find_matching_delimiter(const std::string &src, size_t open_pos,
                                      char open_ch, char close_ch) noexcept {
  int depth = 0;
  size_t i = open_pos;
  while (i < src.size()) {
    char c = src[i];
    if (c == '"' || c == '\'') {
      char quote = c;
      ++i;
      while (i < src.size() && src[i] != quote) {
        if (src[i] == '\\')
          ++i;
        ++i;
      }
    } else if (c == '/' && src[i + 1] == '/') {
      i += 2;
      while (i < src.size() && src[i] != '\n')
        ++i;
    } else if (c == '/' && src[i + 1] == '*') {
      i += 2;
      while (i + 1 < src.size() && !(src[i] == '*' && src[i + 1] == '/'))
        ++i;
      ++i;
    } else if (c == open_ch) {
      ++depth;
    } else if (c == close_ch) {
      --depth;
      if (depth == 0)
        return i;
    }
    ++i;
  }
  return std::string::npos;
}

/// @brief Extract trailing comment (`/* ... */` or consecutive `///` / `//`
/// lines) immediately preceding `decl_start`.
/// @param prefix The substring before the declaration.
/// @return Pair of (leading_trivia_before_comment, extracted_docstring).
static std::pair<std::string, std::string>
split_leading_docstring(const std::string &prefix) noexcept {
  std::pair<std::string, std::string> out{prefix, ""};
  size_t end_ws = prefix.find_last_not_of(" \t\r\n");
  if (end_ws == std::string::npos) {
    return out;
  }

  // Case 1: Ends with `*/` block comment
  if (std::string_view(prefix.data(), end_ws + 1).ends_with("*/")) {
    size_t start_comment = prefix.rfind("/*", end_ws - 1);
    out = {prefix.substr(0, start_comment),
           prefix.substr(start_comment, end_ws - start_comment + 1)};
  } else {
    // Case 2: Ends with consecutive `///` or `//` line comments
    std::vector<std::string> lines;
    size_t pos = 0;
    std::vector<size_t> line_starts;
    for (;;) {
      line_starts.push_back(pos);
      size_t nl = prefix.find('\n', pos);
      if (nl == std::string::npos) {
        lines.push_back(prefix.substr(pos));
        break;
      }
      lines.push_back(prefix.substr(pos, nl - pos + 1));
      pos = nl + 1;
    }

    int idx = static_cast<int>(lines.size()) - 1;
    while (lines[idx].find_first_not_of(" \t\r\n") == std::string::npos) {
      --idx;
    }

    int doc_end_line = idx;
    int doc_start_line = idx + 1;
    while (idx >= 0) {
      size_t first_ch = lines[idx].find_first_not_of(" \t\r\n");
      if (first_ch != std::string::npos &&
          lines[idx].substr(first_ch).starts_with("//")) {
        doc_start_line = idx;
        --idx;
      } else {
        break;
      }
    }

    if (doc_start_line <= doc_end_line) {
      size_t byte_start =
          prefix.find_first_not_of(" \t", line_starts[doc_start_line]);
      std::string doc = prefix.substr(byte_start, end_ws - byte_start + 1);
      out = {prefix.substr(0, byte_start), doc};
    }
  }

  return out;
}

/// @brief Normalize a C++ parameter or field type string (strip leading `const`
/// and trailing `&` / `*` / spaces).
/// @param raw_type The raw type string.
/// @return Cleaned C++ type string.
static std::string clean_cpp_type(std::string raw_type) noexcept {
  auto trim = [](std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t last = s.find_last_not_of(" \t\r\n");
    s = s.substr(start, last - start + 1);
  };
  trim(raw_type);
  if (raw_type.starts_with("const ")) {
    raw_type = raw_type.substr(6);
    trim(raw_type);
  }
  while (raw_type.back() == '&' || raw_type.back() == '*' ||
         std::isspace(static_cast<unsigned char>(raw_type.back()))) {
    raw_type.pop_back();
  }
  trim(raw_type);
  return raw_type;
}

/// @brief Parse a comma-separated parameter list (respecting `<...>` template
/// commas) into `CppField` entries.
/// @param params_str The raw parameter list inside `(...)`.
/// @return Vector of parsed CppField parameter objects.
static std::vector<CppField>
parse_param_list(const std::string &params_str) noexcept {
  std::vector<CppField> params;
  int angle_depth = 0;
  size_t seg_start = 0;

  auto process_segment = [&](const std::string &seg) {
    size_t first = seg.find_first_not_of(" \t\r\n");
    if (first != std::string::npos) {
      std::string leading = seg.substr(0, first);
      std::string trimmed = seg.substr(first);

      // Strip default value `= ...` if present
      size_t eq_pos = trimmed.find('=');
      if (eq_pos != std::string::npos) {
        trimmed = trimmed.substr(0, eq_pos);
      }
      trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

      // Extract trailing identifier as parameter name
      size_t name_end = trimmed.size();
      size_t name_start = name_end;
      while (name_start > 0 && is_ident_char(trimmed[name_start - 1])) {
        --name_start;
      }
      if (name_start > 0 && name_start < name_end) {
        std::string p_name = trimmed.substr(name_start, name_end - name_start);
        std::string p_type = clean_cpp_type(trimmed.substr(0, name_start));

        CppField p;
        p.type = p_type;
        p.name = p_name;
        p.trivia.leading = leading;
        params.push_back(p);
      }
    }
  };

  for (size_t i = 0; i < params_str.size(); ++i) {
    char c = params_str[i];
    if (c == '<') {
      ++angle_depth;
    } else if (c == '>' && angle_depth > 0) {
      --angle_depth;
    } else if (c == ',' && angle_depth == 0) {
      process_segment(params_str.substr(seg_start, i - seg_start));
      seg_start = i + 1;
    }
  }
  if (seg_start < params_str.size()) {
    process_segment(params_str.substr(seg_start));
  }
  return params;
}

/// @brief Parse fields inside a struct/class body string without regexes.
/// @param body The source inside `struct Name { ... }`.
/// @return Vector of parsed CppField objects.
static std::vector<CppField>
parse_class_fields(const std::string &body) noexcept {
  std::vector<CppField> fields;
  size_t pos = 0;
  for (;;) {
    size_t semi = std::string::npos;
    int brace_depth = 0;
    for (size_t i = pos; i < body.size(); ++i) {
      char c = body[i];
      if (c == '/' && body[i + 1] == '/') {
        i = body.find('\n', i + 2);
      } else if (c == '/' && body[i + 1] == '*') {
        i = body.find("*/", i + 2) + 1;
      } else if (c == '{') {
        ++brace_depth;
      } else if (c == '}') {
        --brace_depth;
      } else if (c == ';' && brace_depth == 0) {
        semi = i;
        break;
      }
    }
    if (semi == std::string::npos)
      break;

    std::string inline_doc;
    size_t after_semi = semi + 1;
    while (body[after_semi] == ' ' || body[after_semi] == '\t') {
      ++after_semi;
    }
    if (body.substr(after_semi, 2) == "//") {
      size_t nl = body.find_first_of("\r\n", after_semi);
      inline_doc = body.substr(after_semi, nl - after_semi);
      after_semi = nl;
    } else {
      after_semi = semi + 1;
    }

    std::string segment = body.substr(pos, semi - pos);
    size_t decl_offset = 0;
    size_t scan = 0;
    for (;;) {
      scan = segment.find_first_not_of(" \t\r\n", scan);
      if (segment.substr(scan, 2) == "//") {
        scan = segment.find('\n', scan + 2);
      } else if (segment.substr(scan, 2) == "/*") {
        scan = segment.find("*/", scan + 2) + 2;
      } else {
        size_t word_end = scan;
        while (is_ident_char(segment[word_end]))
          ++word_end;
        if (segment[word_end] == ':' && segment[word_end + 1] != ':') {
          scan = word_end + 1;
        } else {
          decl_offset = scan;
          break;
        }
      }
    }

    std::string raw_prefix = segment.substr(0, decl_offset);
    std::string decl = segment.substr(decl_offset);

    size_t eq = decl.find('=');
    if (eq != std::string::npos) {
      decl = decl.substr(0, eq);
    }
    size_t last_ns = decl.find_last_not_of(" \t\r\n");
    if (decl.find('(') == std::string::npos) {
      decl = decl.substr(0, last_ns + 1);
      size_t name_end = decl.size();
      size_t name_start = name_end;
      while (is_ident_char(decl[name_start - 1])) {
        --name_start;
      }
      std::string fname = decl.substr(name_start);
      std::string ftype = clean_cpp_type(decl.substr(0, name_start));
      if (!kSkipFieldTypes.contains(ftype)) {
        CppField fld;
        fld.type = ftype;
        fld.name = fname;
        auto [lead_trivia, pre_doc] = split_leading_docstring(raw_prefix);
        fld.trivia.leading = lead_trivia;
        if (!inline_doc.empty()) {
          fld.docstring = inline_doc;
        } else if (!pre_doc.empty()) {
          fld.docstring = pre_doc;
        }
        fields.push_back(fld);
      }
    }

    pos = after_semi;
  }
  return fields;
}

/// @brief Locate the next `struct Name {` or `class Name {` header in `source`.
/// @param source Full C++ source string.
/// @param search_pos Starting byte offset.
/// @param out_match_start Populated with byte index of `struct` or `class`.
/// @param out_open_brace Populated with byte index of `{`.
/// @param out_name Populated with the struct/class identifier.
/// @return True if a struct/class definition header was found.
static bool find_next_class_head(const std::string &source, size_t search_pos,
                                 size_t &out_match_start,
                                 size_t &out_open_brace,
                                 std::string &out_name) noexcept {
  size_t i = search_pos;
  while (i < source.size()) {
    char c = source[i];
    if (c == '/' && source[i + 1] == '/') {
      while (i < source.size() && source[i] != '\n')
        ++i;
    } else if (c == '/' && source[i + 1] == '*') {
      i += 2;
      while (i + 1 < source.size() &&
             !(source[i] == '*' && source[i + 1] == '/'))
        ++i;
      ++i;
    } else if ((i == 0 || !is_ident_char(source[i - 1])) &&
               (std::string_view(source).substr(i).starts_with("struct ") ||
                std::string_view(source).substr(i).starts_with("class "))) {
      size_t kw_len =
          std::string_view(source).substr(i).starts_with("struct ") ? 7 : 6;
      size_t ns = source.find_first_not_of(" \t\r\n", i + kw_len);
      size_t ne = ns;
      while (is_ident_char(source[ne]))
        ++ne;
      size_t after_name = source.find_first_not_of(" \t\r\n", ne);
      if (after_name != std::string::npos &&
          (source[after_name] == '{' || source[after_name] == ':')) {
        size_t brace = source.find_first_of("{;", after_name);
        out_match_start = i;
        out_open_brace = brace;
        out_name = source.substr(ns, ne - ns);
        return true;
      }
    }
    ++i;
  }
  return false;
}

/// @brief Locate the next function header `RetType name(` in `source`.
/// @param source Full C++ source string.
/// @param search_pos Starting byte offset.
/// @param out_match_start Populated with byte index where return type starts.
/// @param out_open_paren Populated with byte index of `(`.
/// @param out_ret_type Populated with return type string.
/// @param out_fn_name Populated with function name string.
/// @return True if a function header was found.
static bool find_next_func_head(const std::string &source, size_t search_pos,
                                size_t &out_match_start, size_t &out_open_paren,
                                std::string &out_ret_type,
                                std::string &out_fn_name) noexcept {
  size_t pos = search_pos;
  for (;;) {
    size_t open_p = source.find('(', pos);
    if (open_p == std::string::npos)
      return false;

    size_t ne = open_p;
    size_t ns = ne;
    while (ns > search_pos &&
           (is_ident_char(source[ns - 1]) || source[ns - 1] == ':')) {
      --ns;
    }

    size_t te = ns;
    while (te > search_pos &&
           (std::isspace(static_cast<unsigned char>(source[te - 1])) ||
            source[te - 1] == '&')) {
      --te;
    }
    size_t ts = te;
    if (ts > search_pos && source[ts - 1] == '>') {
      int depth = 1;
      --ts;
      while (ts > search_pos && depth > 0) {
        --ts;
        if (source[ts] == '>')
          ++depth;
        else if (source[ts] == '<')
          --depth;
      }
    }
    while (ts > search_pos &&
           (is_ident_char(source[ts - 1]) || source[ts - 1] == ':')) {
      --ts;
    }
    if (ns == ne || ts == te) {
      pos = open_p + 1;
      continue;
    }

    std::string ret_type = source.substr(ts, te - ts);
    std::string fn_name = source.substr(ns, ne - ns);

    size_t ms = ts;
    for (;;) {
      size_t prev_e = ms;
      while (prev_e > search_pos &&
             std::isspace(static_cast<unsigned char>(source[prev_e - 1])))
        --prev_e;
      size_t prev_s = prev_e;
      while (prev_s > search_pos && is_ident_char(source[prev_s - 1]))
        --prev_s;
      std::string_view word(source.data() + prev_s, prev_e - prev_s);
      if (word == "inline" || word == "virtual" || word == "static") {
        ms = prev_s;
      } else {
        break;
      }
    }

    out_match_start = ms;
    out_open_paren = open_p;
    out_ret_type = ret_type;
    out_fn_name = fn_name;
    return true;
  }
}

/// @brief Process C++ AST representation via `parse_cpp`.
CppAST parse_cpp(const std::string &source) noexcept {
  CppAST ast;

  // 1. Parse structs and classes with balanced `{ ... }`
  size_t search_pos = 0;
  size_t match_start = 0;
  size_t open_brace = 0;
  std::string cls_name;

  while (find_next_class_head(source, search_pos, match_start, open_brace,
                              cls_name)) {
    size_t close_brace = find_matching_delimiter(source, open_brace, '{', '}');
    if (close_brace == std::string::npos) {
      break;
    }

    std::string raw_prefix =
        source.substr(search_pos, match_start - search_pos);
    auto [lead_trivia, doc] = split_leading_docstring(raw_prefix);

    CppClass cls;
    cls.name = cls_name;
    cls.docstring = doc;
    cls.trivia.leading = lead_trivia;

    std::string body =
        source.substr(open_brace + 1, close_brace - open_brace - 1);
    cls.fields = parse_class_fields(body);

    ast.classes.push_back(cls);
    search_pos = close_brace + 1;
  }

  // 2. Parse functions with balanced `( ... )` and balanced `{ ... }` or `;`
  size_t fn_search_pos = 0;
  size_t open_paren = 0;
  std::string ret_type;
  std::string fn_name;

  while (find_next_func_head(source, fn_search_pos, match_start, open_paren,
                             ret_type, fn_name)) {
    size_t close_paren = find_matching_delimiter(source, open_paren, '(', ')');
    if (close_paren == std::string::npos) {
      break;
    }

    size_t scope_pos = fn_name.rfind("::");
    if (scope_pos != std::string::npos) {
      fn_name = fn_name.substr(scope_pos + 2);
    }

    size_t after_paren = close_paren + 1;
    while (after_paren < source.size() && source[after_paren] != '{' &&
           source[after_paren] != ';') {
      ++after_paren;
    }

    std::string raw_prefix =
        source.substr(fn_search_pos, match_start - fn_search_pos);
    auto [lead_trivia, doc] = split_leading_docstring(raw_prefix);

    CppFunction fn;
    fn.return_type = ret_type;
    fn.name = fn_name;
    fn.docstring = doc;
    fn.trivia.leading = lead_trivia;
    fn.params = parse_param_list(
        source.substr(open_paren + 1, close_paren - open_paren - 1));

    if (source[after_paren] == '{') {
      size_t close_brace =
          find_matching_delimiter(source, after_paren, '{', '}');
      if (close_brace == std::string::npos) {
        break;
      }
      fn.body = source.substr(after_paren + 1, close_brace - after_paren - 1);
      fn_search_pos = close_brace + 1;
    } else {
      fn_search_pos = after_paren + 1;
    }

    ast.functions.push_back(fn);
  }

  return ast;
}

/// @brief Process C++ AST representation via `emit_cpp`.
std::string emit_cpp(const CppAST &ast) noexcept {
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
