#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::routes {

std::string emit(const openapi::OpenAPI& spec) {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <string>\n";
  ss << "#include <functional>\n";
  ss << "#include <map>\n\n";
  ss << "namespace cdd_server {\n\n";

  ss << "    class Router {\n";
  ss << "    public:\n";
  ss << "        using Handler = std::function<std::string(const std::string&)>;\n\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_method = [&](const std::string &method,
                             const std::optional<openapi::Operation> &op) {
        if (!op.has_value()) return;
        std::string func_name = op->operationId.value_or("handler");
        ss << "        void on_" << method << "_" << func_name 
           << "(const std::string& path, Handler handler) {\n";
        ss << "            routes[\"" << method << " \" + path] = handler;\n";
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

} // namespace cdd_cpp::routes
