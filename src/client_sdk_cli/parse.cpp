/// @file parse.cpp
/// @brief Implementation of C++ CLI application source parser into
/// OpenAPI 3.2.0.

#include "parse.hpp"
#include "../docstrings/parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>

namespace cdd_cpp::client_sdk_cli {

/// @brief Strip leading whitespace and `///` or `//` markers from a line.
/// @param line Raw line string view.
/// @return Trimmed line content after comment markers.
static std::string strip_line_comment(std::string_view line) noexcept {
  size_t first = line.find_first_not_of(" \t");
  if (first == std::string_view::npos)
    return "";
  std::string_view rem = line.substr(first);
  if (rem.starts_with("///"))
    rem.remove_prefix(3);
  else if (rem.starts_with("//"))
    rem.remove_prefix(2);
  else
    return "";
  size_t s = rem.find_first_not_of(" \t");
  size_t e = rem.find_last_not_of(" \t\r\n");
  return std::string(rem.substr(s, e - s + 1));
}

/// @brief Parse `@param`, `@param_example`, and `@param_deprecated` tags from a
/// docstring or comment block.
/// @param block Source docstring or comment block.
/// @param p_descs Map populated with parameter descriptions.
/// @param p_examples Map populated with parameter examples.
/// @param p_deps Set populated with deprecated parameter names.
static void parse_param_tags(const std::string &block,
                             std::map<std::string, std::string> &p_descs,
                             std::map<std::string, std::string> &p_examples,
                             std::set<std::string> &p_deps) noexcept {
  std::istringstream iss(block);
  std::string line;
  while (std::getline(iss, line)) {
    std::string content = strip_line_comment(line);
    if (content.starts_with("@param ")) {
      std::string rest = content.substr(7);
      size_t s1 = rest.find_first_not_of(" \t");
      size_t e1 = rest.find_first_of(" \t", s1);
      if (e1 != std::string::npos) {
        std::string pname = rest.substr(s1, e1 - s1);
        size_t s2 = rest.find_first_not_of(" \t", e1);
        p_descs[pname] = rest.substr(s2);
      }
    } else if (content.starts_with("@param_example ")) {
      std::string rest = content.substr(15);
      size_t s1 = rest.find_first_not_of(" \t");
      size_t e1 = rest.find_first_of(" \t", s1);
      std::string pname = rest.substr(s1, e1 - s1);
      size_t s2 = rest.find_first_not_of(" \t", e1);
      p_examples[pname] = rest.substr(s2);
    } else if (content.starts_with("@param_deprecated ")) {
      std::string rest = content.substr(18);
      size_t s1 = rest.find_first_not_of(" \t");
      size_t e1 = rest.find_first_of(" \t\r\n", s1);
      p_deps.insert(rest.substr(s1, e1 - s1));
    }
  }
}

/// @brief Internal helper or implementation for `parse`.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed CLI";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  docstrings::parse_api_docstrings(input, spec);

  // 1. Parse modern `handle_<op_id>(cdd_cli::Client& client, ...)` functions
  // via CppAST
  utils::CppAST ast = utils::parse_cpp(input);

  for (const auto &fn : ast.functions) {
    if (!fn.name.starts_with("handle_"))
      continue;

    std::string op_id = fn.name.substr(7);
    openapi::Operation op;
    op.operationId = op_id;
    docstrings::parse_operation_docstrings(fn.docstring, op);

    std::string method = "POST";
    std::string path = "/" + op_id;
    if (auto route_val = docstrings::extract_tag(fn.docstring, "route")) {
      size_t sp = route_val->find_first_of(" \t");
      method = route_val->substr(0, sp);
      size_t ps = route_val->find_first_not_of(" \t", sp);
      size_t pe = route_val->find_first_of(" \t\r\n", ps);
      path = route_val->substr(ps, pe - ps);
    }

    std::map<std::string, std::string> p_descs;
    std::map<std::string, std::string> p_examples;
    std::set<std::string> p_deps;
    parse_param_tags(fn.docstring, p_descs, p_examples, p_deps);

    std::vector<openapi::Parameter> params;
    for (const auto &p : fn.params) {
      if (p.name == "client")
        continue;
      if (p.name == "body") {
        openapi::RequestBody rb;
        rb.description = "CLI body parameter";
        op.requestBody = rb;
      } else {
        openapi::Parameter param;
        param.name = p.name;
        if (path.find("{" + p.name + "}") != std::string::npos) {
          param.in = "path";
          param.required = true;
        } else {
          param.in = "query";
        }
        if (p_descs.contains(p.name))
          param.description = p_descs[p.name];
        if (p_examples.contains(p.name))
          param.example = p_examples[p.name];
        if (p_deps.contains(p.name))
          param.deprecated = true;
        params.push_back(param);
      }
    }
    if (!params.empty())
      op.parameters = params;

    (*spec.paths)[path].set_operation(method, std::move(op));
  }

  // 2. Parse legacy `if (command == "...") { ... }` subcommands
  size_t search_pos = 0;
  for (;;) {
    size_t cmd_idx = input.find("command == \"", search_pos);
    if (cmd_idx == std::string::npos)
      break;
    size_t q1 = cmd_idx + 11;
    size_t q2 = input.find('"', q1 + 1);
    size_t open_b = input.find('{', q2 + 1);
    size_t close_b = input.find('}', open_b + 1);

    std::string cmd = input.substr(q1 + 1, q2 - q1 - 1);
    std::string body = input.substr(open_b + 1, close_b - open_b - 1);

    std::string path = "/" + cmd;
    openapi::PathItem pi;
    openapi::Operation op;
    op.operationId = cmd;

    docstrings::parse_operation_docstrings(body, op);

    std::istringstream bss(body);
    std::string bline;
    while (std::getline(bss, bline)) {
      std::string cmt = strip_line_comment(bline);
      if (!cmt.empty()) {
        op.summary = cmt;
        op.description = cmt;
        break;
      }
    }

    std::map<std::string, std::string> p_descs;
    std::map<std::string, std::string> p_examples;
    std::set<std::string> p_deps;
    parse_param_tags(body, p_descs, p_examples, p_deps);

    std::vector<openapi::Parameter> params;
    size_t p_pos = 0;
    for (;;) {
      size_t flag_idx = body.find("\"--", p_pos);
      if (flag_idx == std::string::npos)
        break;
      size_t flag_end = body.find('"', flag_idx + 3);
      std::string p_name = body.substr(flag_idx + 3, flag_end - flag_idx - 3);
      if (p_name == "body") {
        openapi::RequestBody reqBody;
        reqBody.description = "CLI body parameter";
        op.requestBody = reqBody;
      } else {
        openapi::Parameter p;
        p.name = p_name;
        p.in = "query";
        if (p_descs.contains(p_name))
          p.description = p_descs[p_name];
        if (p_examples.contains(p_name))
          p.example = p_examples[p_name];
        if (p_deps.contains(p_name))
          p.deprecated = true;
        params.push_back(p);
      }
      p_pos = flag_end + 1;
    }

    if (!params.empty()) {
      op.parameters = params;
    }

    pi.post = op;
    spec.paths->insert({path, pi});
    search_pos = close_b + 1;
  }

  return spec;
}

} // namespace cdd_cpp::client_sdk_cli
