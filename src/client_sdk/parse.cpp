/// @file parse.cpp
/// @brief Implementation of C++ client SDK source parser into OpenAPI 3.2.0.

#include "parse.hpp"
#include "../docstrings/parse.hpp"
#include "../models/parse.hpp"
#include "../utils/cpp_ast.hpp"
#include <optional>
#include <string>
#include <string_view>

namespace cdd_cpp::client_sdk {

/// @brief Extract a double-quoted string literal following `prefix_key` in
/// `body`.
/// @param body Source code body to search.
/// @param prefix_key Substring preceding the quoted literal (e.g., `base_url`
/// or `CURLOPT_CUSTOMREQUEST`).
/// @return Optional extracted string literal.
static std::optional<std::string>
extract_quoted_after(std::string_view body,
                     std::string_view prefix_key) noexcept {
  size_t pos = body.find(prefix_key);
  if (pos == std::string_view::npos)
    return std::nullopt;
  size_t q1 = body.find('"', pos + prefix_key.size());
  size_t q2 = body.find('"', q1 + 1);
  return std::string(body.substr(q1 + 1, q2 - q1 - 1));
}

/// @brief Internal helper or implementation for `parse`.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed Client SDK";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  utils::CppAST ast = utils::parse_cpp(input);
  utils::parse_schemas(ast, spec);

  docstrings::parse_api_docstrings(input, spec);

  for (const auto &cls : ast.classes) {
    if (cls.name == "Client") {
      docstrings::parse_api_docstrings(cls.docstring, spec);
    }
  }

  for (const auto &fn : ast.functions) {
    if (fn.name == "WriteCallback")
      continue;

    std::string method =
        extract_quoted_after(fn.body, "CURLOPT_CUSTOMREQUEST").value_or("GET");
    std::string path = extract_quoted_after(fn.body, "base_url +").value_or("");

    if (!path.empty()) {
      openapi::Operation op;
      op.operationId = fn.name;

      docstrings::parse_operation_docstrings(fn.docstring, op);

      std::vector<openapi::Parameter> params;
      for (const auto &param : fn.params) {
        if (param.name == "body") {
          openapi::RequestBody req_body;
          req_body.description = "Parsed request body";
          if (auto v = docstrings::extract_tag(fn.docstring, "param body")) {
            size_t dash = v->find(" - ");
            if (dash != std::string::npos) {
              req_body.description = v->substr(dash + 3);
            }
          }
          req_body.content = std::map<std::string, openapi::MediaType>{
              {"application/json", {}}};
          op.requestBody = req_body;
        } else {
          openapi::Parameter p;
          p.name = param.name;
          if (path.find("{" + param.name + "}") != std::string::npos ||
              path.find("<" + param.name + ">") != std::string::npos) {
            p.in = "path";
            p.required = true;
          } else if (fn.body.find(std::string("\"") + param.name + ": \"") !=
                     std::string::npos) {
            p.in = "header";
          } else {
            p.in = "query";
          }

          p.schema = openapi::Schema{};
          if (param.type == "int")
            p.schema->type = "integer";
          else if (param.type == "double")
            p.schema->type = "number";
          else if (param.type == "bool")
            p.schema->type = "boolean";
          else
            p.schema->type = "string";

          params.push_back(p);
        }
      }
      if (!params.empty()) {
        op.parameters = params;
      }

      (*spec.paths)[path].set_operation(method, std::move(op));
    }
  }

  return spec;
}

/// @brief Internal helper or implementation for `parse_arazzo`.
Result<arazzo::Arazzo> parse_arazzo(const std::string &input) noexcept {
  if (input.empty()) {
    return utils::make_error(utils::ErrorCode::CppAstParseError,
                             "Empty client SDK workflow input");
  }

  arazzo::Arazzo spec;
  spec.arazzo = "1.1.0";
  spec.info.title = "Parsed SDK Workflows";
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

  if (input.find("class Workflows") == std::string::npos) {
    return spec;
  }

  utils::CppAST ast = utils::parse_cpp(input);
  for (const auto &fn : ast.functions) {
    arazzo::Workflow wf;
    wf.workflowId = fn.name;
    docstrings::parse_workflow_docstrings(fn.docstring, wf);
    arazzo::Step st;
    st.stepId = "step_1";
    st.operationId = fn.name;
    wf.steps.push_back(std::move(st));
    spec.workflows.push_back(std::move(wf));
  }

  return spec;
}

} // namespace cdd_cpp::client_sdk
