/// @file parse.cpp
/// @brief Implementation of standalone C++ function declarations parser.

#include "parse.hpp"
#include "../docstrings/parse.hpp"
#include "../utils/cpp_ast.hpp"

namespace cdd_cpp::functions {

/// @brief Parse standalone C++ function declarations for `parse`.
Result<openapi::OpenAPI> parse(const std::string &input) noexcept {
  if (input.empty()) {
    return utils::make_error(utils::ErrorCode::CppAstParseError,
                             "Empty function source input");
  }

  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Parsed Functions";
  spec.info.version = "1.0.0";
  spec.paths = std::map<std::string, openapi::PathItem>{};

  docstrings::parse_api_docstrings(input, spec);

  utils::CppAST ast = utils::parse_cpp(input);

  for (const auto &fn : ast.functions) {
    openapi::Operation op;
    op.operationId = fn.name;
    docstrings::parse_operation_docstrings(fn.docstring, op);

    std::string method = "GET";
    std::string path = "/" + fn.name;
    if (auto route_val = docstrings::extract_tag(fn.docstring, "route")) {
      size_t sp = route_val->find_first_of(" \t");
      method = route_val->substr(0, sp);
      size_t ps = route_val->find_first_not_of(" \t", sp);
      size_t pe = route_val->find_first_of(" \t\r\n", ps);
      path = route_val->substr(ps, pe - ps);
    }

    std::vector<openapi::Parameter> params;
    for (const auto &p : fn.params) {
      if (p.name == "body") {
        openapi::RequestBody rb;
        rb.required = true;
        op.requestBody = rb;
      } else {
        openapi::Parameter param;
        param.name = p.name;
        param.in = (path.find("{" + p.name + "}") != std::string::npos)
                       ? "path"
                       : "query";
        param.required = (param.in == "path");
        param.schema = openapi::Schema{};
        param.schema->type = openapi::cpp_type_to_openapi(p.type);
        params.push_back(param);
      }
    }
    if (!params.empty())
      op.parameters = params;

    (*spec.paths)[path].set_operation(method, std::move(op));
  }

  return spec;
}

/// @brief Parse standalone C++ function declarations for `parse_arazzo`.
Result<arazzo::Arazzo> parse_arazzo(const std::string &input) noexcept {
  if (input.empty()) {
    return utils::make_error(utils::ErrorCode::CppAstParseError,
                             "Empty function source input");
  }

  arazzo::Arazzo spec;
  spec.arazzo = "1.1.0";
  spec.info.title = "Parsed Workflows";
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

  utils::CppAST ast = utils::parse_cpp(input);
  for (const auto &fn : ast.functions) {
    arazzo::Workflow wf;
    wf.workflowId = fn.name;
    docstrings::parse_workflow_docstrings(fn.docstring, wf);
    openapi::Schema in_schema;
    in_schema.type = "object";
    in_schema.properties =
        std::make_shared<std::map<std::string, openapi::Schema>>();
    for (const auto &p : fn.params) {
      openapi::Schema ps;
      ps.type = openapi::cpp_type_to_openapi(p.type);
      (*in_schema.properties)[p.name] = std::move(ps);
    }
    wf.inputs = std::move(in_schema);
    arazzo::Step st;
    st.stepId = "step_1";
    st.operationId = fn.name;
    wf.steps.push_back(std::move(st));
    spec.workflows.push_back(std::move(wf));
  }

  return spec;
}

} // namespace cdd_cpp::functions
