#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::mocks {
std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;

  ss << "#pragma once\n";
  ss << "#include <string>\n";
  ss << "#include <gmock/gmock.h>\n\n";

  ss << "namespace cdd_mock_client {\n\n";

  // Generate Interface
  ss << "    class IClient {\n";
  ss << "    public:\n";
  ss << "        virtual ~IClient() = default;\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_interface_method =
          [&](const std::optional<openapi::Operation> &op) {
            if (!op.has_value())
              return;
            std::string func_name = op->operationId.value_or("request");
            ss << "        virtual std::string " << func_name << "() = 0;\n";
          };

      emit_interface_method(item.get);
      emit_interface_method(item.post);
      emit_interface_method(item.put);
      emit_interface_method(item.delete_op);
      emit_interface_method(item.patch);
    }
  }
  ss << "    };\n\n";

  // Generate GMock implementation
  ss << "    class MockClient : public IClient {\n";
  ss << "    public:\n";
  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.value()) {
      auto emit_mock_method = [&](const std::optional<openapi::Operation> &op) {
        if (!op.has_value())
          return;
        std::string func_name = op->operationId.value_or("request");
        ss << "        MOCK_METHOD(std::string, " << func_name
           << ", (), (override));\n";
      };

      emit_mock_method(item.get);
      emit_mock_method(item.post);
      emit_mock_method(item.put);
      emit_mock_method(item.delete_op);
      emit_mock_method(item.patch);
    }
  }
  ss << "    };\n\n";

  ss << "}\n";

  return ss.str();
}
} // namespace cdd_cpp::mocks
