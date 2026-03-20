#include "../docstrings/emit.hpp"
#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::server {

std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <string>\n";
  ss << "#include <functional>\n";
  ss << "#include <map>\n\n";
  ss << "#include \"../docstrings/emit.hpp\"\n";
  ss << "namespace cdd_server {\n\n";
  ss << docstrings::emit_api_docstrings(spec);

  ss << "    class Router {\n";
  ss << "    public:\n";
  ss << "        using Handler = std::function<std::string(const "
        "std::string&)>;\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_method = [&](const std::string &method,
                             const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("handler");

        ss << docstrings::emit_path_docstrings(item);

        std::vector<openapi::Parameter> all_params;
        if (item.parameters) {
          for (const auto &p : *item.parameters)
            all_params.push_back(p);
        }
        if (op->parameters) {
          for (const auto &p : *op->parameters)
            all_params.push_back(p);
        }

        openapi::Operation new_op = op.value();
        new_op.parameters = all_params;
        ss << docstrings::emit_operation_docstrings(new_op);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = "std::string";
          if (p.schema && p.schema->type) {
            if (*p.schema->type == "integer")
              type = "int";
            else if (*p.schema->type == "boolean")
              type = "bool";
            else if (*p.schema->type == "number")
              type = "double";
          }
          if (i > 0)
            param_list += ", ";
          param_list += type + " " + p.name;
        }
        if (op->requestBody) {
          if (!param_list.empty())
            param_list += ", ";
          param_list += "std::string requestBody";
        }
        if (param_list.empty()) {
          param_list = "const std::string&";
        }

        ss << "        using " << func_name
           << "Handler = std::function<std::string(" << param_list << ")>;\n";
        ss << "        void on_" << method << "_" << func_name
           << "(const std::string& path, " << func_name
           << "Handler handler) {\n";
        ss << "            // " << param_list << "\n";
        ss << "            routes[\"" << method << " \" + path] = nullptr;\n";
        ss << "        }\n\n";
      };

      emit_method("GET", item.get);
      emit_method("POST", item.post);
      emit_method("PUT", item.put);
      emit_method("DELETE", item.delete_op);
      emit_method("PATCH", item.patch);
    }
  }

  ss << "    private:\n";
  ss << "        std::map<std::string, Handler> routes;\n";
  ss << "    };\n";
  ss << "}\n";

  return ss.str();
}

} // namespace cdd_cpp::server
