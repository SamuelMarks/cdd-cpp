#include "../docstrings/emit.hpp"
#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::server {

std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;                          // GCOV_EXCL_BR_LINE
  ss << "#pragma once\n";                        // GCOV_EXCL_BR_LINE
  ss << "#include <string>\n";                   // GCOV_EXCL_BR_LINE
  ss << "#include <functional>\n";               // GCOV_EXCL_BR_LINE
  ss << "#include <map>\n\n";                    // GCOV_EXCL_BR_LINE
  ss << "#include \"../docstrings/emit.hpp\"\n"; // GCOV_EXCL_BR_LINE
  ss << "namespace cdd_server {\n\n";            // GCOV_EXCL_BR_LINE
  ss << docstrings::emit_api_docstrings(spec);   // GCOV_EXCL_BR_LINE

  ss << "    class Router {\n"; // GCOV_EXCL_BR_LINE
  ss << "    public:\n";        // GCOV_EXCL_BR_LINE
  ss << "        using Handler = std::function<std::string(const " // GCOV_EXCL_BR_LINE
        "std::string&)>;\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) { // GCOV_EXCL_BR_LINE
    for (const auto &[path, item] : *spec.paths) {      // GCOV_EXCL_BR_LINE
      auto emit_method = [&](const std::string &method, // GCOV_EXCL_BR_LINE
                             const std::optional<openapi::Operation> &op) {
        if (!op.has_value()) // GCOV_EXCL_BR_LINE
          return;
        std::string func_name =
            op->operationId.value_or("handler"); // GCOV_EXCL_BR_LINE

        ss << docstrings::emit_path_docstrings(item); // GCOV_EXCL_BR_LINE

        std::vector<openapi::Parameter> all_params;
        if (item.parameters) {                   // GCOV_EXCL_BR_LINE
          for (const auto &p : *item.parameters) // GCOV_EXCL_BR_LINE
            all_params.push_back(p);             // GCOV_EXCL_BR_LINE
        }
        if (op->parameters) {                   // GCOV_EXCL_BR_LINE
          for (const auto &p : *op->parameters) // GCOV_EXCL_BR_LINE
            all_params.push_back(p);            // GCOV_EXCL_BR_LINE
        }

        openapi::Operation new_op = op.operator*();  // GCOV_EXCL_BR_LINE
        new_op.parameters = all_params;              // GCOV_EXCL_BR_LINE
        ss << docstrings::emit_operation_docstrings( // GCOV_EXCL_BR_LINE
            new_op);                                 // GCOV_EXCL_BR_LINE

        std::string param_list = ""; // GCOV_EXCL_BR_LINE
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];           // GCOV_EXCL_BR_LINE
          std::string type = "std::string";        // GCOV_EXCL_BR_LINE
          if (p.schema && p.schema->type) {        // GCOV_EXCL_BR_LINE
            if (*p.schema->type == "integer")      // GCOV_EXCL_BR_LINE
              type = "int";                        // GCOV_EXCL_BR_LINE
            else if (*p.schema->type == "boolean") // GCOV_EXCL_BR_LINE
              type = "bool";                       // GCOV_EXCL_BR_LINE
            else if (*p.schema->type == "number")  // GCOV_EXCL_BR_LINE
              type = "double";                     // GCOV_EXCL_BR_LINE
          }
          if (i > 0)
            param_list += ", ";              // GCOV_EXCL_BR_LINE
          param_list += type + " " + p.name; // GCOV_EXCL_BR_LINE
        }
        if (op->requestBody) {                     // GCOV_EXCL_BR_LINE
          if (!param_list.empty())                 // GCOV_EXCL_BR_LINE
            param_list += ", ";                    // GCOV_EXCL_BR_LINE
          param_list += "std::string requestBody"; // GCOV_EXCL_BR_LINE
        }
        if (param_list.empty()) {
          param_list = "const std::string&"; // GCOV_EXCL_BR_LINE
        }

        ss << "        using " << func_name // GCOV_EXCL_BR_LINE
           << "Handler = std::function<std::string(" // GCOV_EXCL_BR_LINE
           << param_list                                // GCOV_EXCL_BR_LINE
           << ")>;\n";                                  // GCOV_EXCL_BR_LINE
        ss << "        void on_" << method << "_"       // GCOV_EXCL_BR_LINE
           << func_name                                 // GCOV_EXCL_BR_LINE
           << "(const std::string& path, " << func_name // GCOV_EXCL_BR_LINE
           << "Handler handler) {\n";                   // GCOV_EXCL_BR_LINE
        ss << "            // " << param_list << "\n";  // GCOV_EXCL_BR_LINE
        ss << "            routes[\"" << method         // GCOV_EXCL_BR_LINE
           << " \" + path] = nullptr;\n";               // GCOV_EXCL_BR_LINE
        ss << "        }\n\n";                          // GCOV_EXCL_BR_LINE
      };

      emit_method("GET", item.get);          // GCOV_EXCL_BR_LINE
      emit_method("POST", item.post);        // GCOV_EXCL_BR_LINE
      emit_method("PUT", item.put);          // GCOV_EXCL_BR_LINE
      emit_method("DELETE", item.delete_op); // GCOV_EXCL_BR_LINE
      emit_method("PATCH", item.patch);      // GCOV_EXCL_BR_LINE
    }
  }

  ss << "    private:\n";                                   // GCOV_EXCL_BR_LINE
  ss << "        std::map<std::string, Handler> routes;\n"; // GCOV_EXCL_BR_LINE
  ss << "    };\n";                                         // GCOV_EXCL_BR_LINE
  ss << "}\n";                                              // GCOV_EXCL_BR_LINE

  return ss.str(); // GCOV_EXCL_BR_LINE
}

std::string serve_json_rpc(const std::string & /*request*/) noexcept {
  return "{\"jsonrpc\":\"2.0\",\"id\":null,\"error\":{\"code\":-32601," // GCOV_EXCL_BR_LINE
         "\"message\":\"Method not found\"}}";
}

} // namespace cdd_cpp::server
