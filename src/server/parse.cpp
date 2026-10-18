/// @file parse.cpp
/// @brief Implementation of C++ server route parser into OpenAPI 3.2.0.

#include "parse.hpp"
#include "../docstrings/parse.hpp"
#include "../models/parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <cctype>
#include <sstream>
#include <string>
#include <string_view>

namespace cdd_cpp::server {

/// @brief Internal helper or implementation for `parse`.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed Routes";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  size_t class_start = input.find('{');
  std::string class_header = input.substr(0, class_start);
  docstrings::parse_api_docstrings(class_header, spec);

  size_t search_pos = 0;
  for (;;) {
    size_t idx = input.find("void on_", search_pos);
    if (idx == std::string::npos)
      break;
    size_t m_start = idx + 8;
    size_t us = input.find('_', m_start);
    size_t lparen = input.find('(', us);
    std::string method = input.substr(m_start, us - m_start);
    std::string func_name = input.substr(us + 1, lparen - us - 1);

    size_t fn_inner = input.find("std::function<std::string(", lparen);
    size_t args_end = input.find(")>", fn_inner + 26);
    size_t open_body = input.find('{', args_end + 2);
    size_t close_body = input.find('}', open_body + 1);
    size_t routes_idx = input.find("routes[\"", open_body);

    std::string args_str =
        input.substr(fn_inner + 26, args_end - (fn_inner + 26));

    if (routes_idx > close_body) {
      search_pos = close_body + 1;
      continue;
    }
    close_body = input.find('}', input.find(';', routes_idx));
    size_t r_start = routes_idx + 8;
    size_t r_end = input.find('"', r_start);
    std::string route_key = input.substr(r_start, r_end - r_start);
    size_t sp = route_key.find(' ');
    std::string path = route_key.substr(sp + 1);

    if (!spec.paths->contains(path)) {
      spec.paths->insert({path, openapi::PathItem{}});
    }

    openapi::Operation op;
    op.operationId = func_name;

    if (!args_str.empty() && args_str != "const std::string&") {
      op.parameters = std::vector<openapi::Parameter>{};
      std::stringstream ass(args_str);
      std::string seg;
      while (std::getline(ass, seg, ',')) {
        size_t s1 = seg.find_first_not_of(" \t\r\n");
        size_t e1 = seg.find_last_not_of(" \t\r\n");
        std::string trimmed = seg.substr(s1, e1 - s1 + 1);
        size_t sep = trimmed.find_last_of(" \t");
        if (sep == std::string::npos)
          continue;
        std::string type_str = trimmed.substr(0, sep);
        std::string name_str = trimmed.substr(sep + 1);

        if (name_str == "requestBody") {
          op.requestBody = openapi::RequestBody{};
        } else {
          openapi::Parameter param;
          param.name = name_str;
          param.in = (path.find("{" + name_str + "}") != std::string::npos)
                         ? "path"
                         : "query";
          param.required = (param.in == "path");
          param.schema = openapi::Schema{};
          if (type_str == "int")
            param.schema->type = "integer";
          else if (type_str == "double")
            param.schema->type = "number";
          else if (type_str == "bool")
            param.schema->type = "boolean";
          else
            param.schema->type = "string";

          op.parameters->push_back(param);
        }
      }

      if (op.parameters->empty()) {
        op.parameters.reset();
      }
    }

    std::string before = input.substr(0, idx);
    size_t last_end = before.find_last_of("{}");
    if (last_end == std::string::npos)
      last_end = 0;
    std::string comments_block = before.substr(last_end);

    docstrings::parse_operation_docstrings(comments_block, op);

    (*spec.paths)[path].set_operation(method, std::move(op));
    search_pos = close_body + 1;
  }

  utils::CppAST ast = utils::parse_cpp(input);
  utils::parse_schemas(ast, spec);
  return spec;
}

/// @brief Internal helper or implementation for `parse_arazzo`.
Result<arazzo::Arazzo> parse_arazzo(const std::string &input) noexcept {
  if (input.empty()) {
    return utils::make_error(utils::ErrorCode::CppAstParseError,
                             "Empty server workflow input");
  }

  arazzo::Arazzo spec;
  spec.arazzo = "1.1.0";
  spec.info.title = "Parsed Server Workflows";
  spec.info.version = "1.0.0";

  docstrings::parse_arazzo_docstrings(input, spec);
  if (!spec.workflows.empty()) {
    return spec;
  }

  arazzo::SourceDescription sd;
  sd.name = "default";
  sd.url = "./openapi.json";
  sd.type = arazzo::SourceDescriptionType::OpenApi;
  spec.sourceDescriptions.push_back(std::move(sd));

  size_t pos = 0;
  for (;;) {
    size_t idx = input.find("void on_workflow_", pos);
    if (idx == std::string::npos) {
      break;
    }
    size_t start = idx + 17;
    size_t lparen = input.find('(', start);
    std::string wf_id = input.substr(start, lparen - start);
    arazzo::Workflow wf;
    wf.workflowId = wf_id;
    arazzo::Step st;
    st.stepId = "step_1";
    st.operationId = wf_id;
    wf.steps.push_back(std::move(st));
    spec.workflows.push_back(std::move(wf));
    pos = lparen + 1;
  }

  return spec;
}

} // namespace cdd_cpp::server
