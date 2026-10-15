/// @file emit.cpp
/// @brief Implementation of standalone C++ function declarations emitter.

#include "emit.hpp"
#include "../docstrings/emit.hpp"
#include <sstream>

namespace cdd_cpp::functions {

/// @brief Internal helper or implementation for `emit`.
std::string emit(const openapi::OpenAPI &spec) noexcept {
  if (!spec.paths.has_value() || spec.paths->empty()) {
    return "{}";
  }

  std::ostringstream ss;
  ss << "#pragma once\n#include <string>\n\nnamespace cdd_functions {\n";

  for (const auto &[path, item] : *spec.paths) {
    auto emit_fn = [&](const std::string &method,
                       const std::optional<openapi::Operation> &op) {
      if (!op.has_value())
        return;
      std::string fn_name = op->operationId.value_or("request");
      ss << "    /// @route " << method << " " << path << "\n";
      ss << docstrings::emit_operation_docstrings(*op);
      ss << "    std::string " << fn_name << "(";
      bool first = true;
      if (op->parameters.has_value()) {
        for (const auto &p : *op->parameters) {
          if (!first)
            ss << ", ";
          first = false;
          std::string ctype = "std::string";
          if (p.schema.has_value() && p.schema->type.has_value()) {
            if (*p.schema->type == "integer")
              ctype = "int";
            else if (*p.schema->type == "number")
              ctype = "double";
            else if (*p.schema->type == "boolean")
              ctype = "bool";
          }
          ss << ctype << " " << p.name;
        }
      }
      if (op->requestBody.has_value()) {
        if (!first)
          ss << ", ";
        ss << "std::string body";
      }
      ss << ");\n";
    };

    emit_fn("GET", item.get);
    emit_fn("POST", item.post);
    emit_fn("PUT", item.put);
    emit_fn("DELETE", item.delete_op);
    emit_fn("PATCH", item.patch);
  }

  ss << "}\n";
  return ss.str();
}

} // namespace cdd_cpp::functions
