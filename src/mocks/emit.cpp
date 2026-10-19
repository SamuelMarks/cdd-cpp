/// @file emit.cpp
/// @brief Implementation of GoogleMock C++ mock client emitter.

#include "emit.hpp"
#include "../docstrings/emit.hpp"
#include <sstream>

namespace cdd_cpp::mocks {

/// @brief Emit GoogleMock interface and mock class definitions for `emit`.
std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;

  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <optional>\n";
  ss << "#include <string>\n";
  ss << "#include <gmock/gmock.h>\n\n";

  ss << "namespace cdd_mock_client {\n\n";

  // Generate Interface
  ss << "    class IClient {\n";
  ss << "    public:\n";
  ss << "        virtual ~IClient() = default;\n";

  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.operator*()) {
      (void)path;
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
      emit_interface_method(item.options);
      emit_interface_method(item.head);
      emit_interface_method(item.trace);
      emit_interface_method(item.query);
    }
  }
  ss << "    };\n\n";

  // Generate GMock implementation
  ss << "    class MockClient : public IClient {\n";
  ss << "    public:\n";
  if (spec.paths.has_value() && !spec.paths->empty()) {
    for (const auto &[path, item] : spec.paths.operator*()) {
      (void)path;
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
      emit_mock_method(item.options);
      emit_mock_method(item.head);
      emit_mock_method(item.trace);
      emit_mock_method(item.query);
    }
  }
  ss << "    };\n\n";

  ss << "}\n";

  return ss.str();
}

/// @brief Emit GoogleMock interface and mock class definitions for
/// `emit_arazzo`.
std::string emit_arazzo(const arazzo::Arazzo &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <optional>\n";
  ss << "#include <string>\n";
  ss << "#include <gmock/gmock.h>\n\n";
  ss << docstrings::emit_arazzo_docstrings(spec);
  ss << "namespace cdd_mock_workflows {\n\n";
  ss << "    class IWorkflows {\n";
  ss << "    public:\n";
  ss << "        virtual ~IWorkflows() = default;\n";
  for (const auto &wf : spec.workflows) {
    ss << docstrings::emit_workflow_docstrings(wf);
    ss << "        virtual std::expected<std::string, std::string> "
       << wf.workflowId << "(const std::string& inputs_json) noexcept = 0;\n";
  }
  ss << "    };\n\n";
  ss << "    class MockWorkflows : public IWorkflows {\n";
  ss << "    public:\n";
  for (const auto &wf : spec.workflows) {
    ss << "        MOCK_METHOD((std::expected<std::string, std::string>), "
       << wf.workflowId
       << ", (const std::string& inputs_json), (noexcept, override));\n";
  }
  ss << "    };\n\n";
  ss << "}\n";
  return ss.str();
}

} // namespace cdd_cpp::mocks
