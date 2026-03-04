#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::orm {

std::string map_type_to_postgres(const std::string &type) noexcept {
  if (type == "string")
    return "TEXT";
  if (type == "integer")
    return "INTEGER";
  if (type == "number")
    return "DOUBLE PRECISION";
  if (type == "boolean")
    return "BOOLEAN";
  return "TEXT";
}

std::string map_type_to_cpp(const std::string &type) noexcept {
  if (type == "string")
    return "std::string";
  if (type == "integer")
    return "int";
  if (type == "number")
    return "double";
  if (type == "boolean")
    return "bool";
  return "std::string";
}

std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <string>\n";
  ss << "#include <vector>\n";
  ss << "#include <libpq-fe.h>\n\n";

  ss << "namespace cdd_orm {\n\n";

  if (spec.components.has_value() && spec.components->schemas.has_value()) {
    for (const auto &[name, schema] : spec.components->schemas.value()) {
      if (schema.type.value_or("") != "object" || !schema.properties)
        continue;

      std::string table_name = name;
      for (auto &c : table_name)
        c = std::tolower(c);

      ss << "    struct " << name << " {\n";
      for (const auto &[prop_name, prop_schema] : *schema.properties) {
        ss << "        " << map_type_to_cpp(prop_schema.type.value_or("string"))
           << " " << prop_name << ";\n";
      }
      ss << "    };\n\n";

      ss << "    class " << name << "Repository {\n";
      ss << "        PGconn* conn;\n";
      ss << "    public:\n";
      ss << "        " << name << "Repository(PGconn* c) : conn(c) {}\n\n";

      // CREATE TABLE
      ss << "        std::expected<void, std::string> create_table() noexcept "
            "{\n";
      ss << "            std::string sql = \"CREATE TABLE IF NOT EXISTS "
         << table_name << " (";

      bool first = true;
      for (const auto &[prop_name, prop_schema] : *schema.properties) {
        if (!first)
          ss << ", ";
        ss << prop_name << " "
           << map_type_to_postgres(prop_schema.type.value_or("string"));
        first = false;
      }
      ss << ")\";\n";
      ss << "            PGresult* res = PQexec(conn, sql.c_str());\n";
      ss << "            if (PQresultStatus(res) != PGRES_COMMAND_OK) {\n";
      ss << "                std::string err = PQerrorMessage(conn);\n";
      ss << "                PQclear(res);\n";
      ss << "                return std::unexpected(err);\n";
      ss << "            }\n";
      ss << "            PQclear(res);\n";
      ss << "            return {};\n";
      ss << "        }\n\n";

      // INSERT
      ss << "        std::expected<void, std::string> insert(const " << name
         << "& item) noexcept {\n";
      ss << "            std::string sql = \"INSERT INTO " << table_name
         << " (";
      first = true;
      for (const auto &[prop_name, prop_schema] : *schema.properties) {
        if (!first)
          ss << ", ";
        ss << prop_name;
        first = false;
      }
      ss << ") VALUES (";
      first = true;
      int idx = 1;
      for (const auto &kv : *schema.properties) {
        (void)kv;
        if (!first)
          ss << ", ";
        ss << "$" << idx++;
        first = false;
      }
      ss << ")\";\n";

      ss << "            const char* paramValues[" << schema.properties->size()
         << "];\n";
      int paramIdx = 0;
      for (const auto &[prop_name, prop_schema] : *schema.properties) {
        std::string c_type =
            map_type_to_cpp(prop_schema.type.value_or("string"));
        if (c_type == "std::string") {
          ss << "            paramValues[" << paramIdx << "] = item."
             << prop_name << ".c_str();\n";
        } else {
          ss << "            std::string p" << paramIdx
             << " = std::to_string(item." << prop_name << ");\n";
          ss << "            paramValues[" << paramIdx << "] = p" << paramIdx
             << ".c_str();\n";
        }
        paramIdx++;
      }

      ss << "            PGresult* res = PQexecParams(conn, sql.c_str(), "
         << schema.properties->size()
         << ", nullptr, paramValues, nullptr, nullptr, 0);\n";
      ss << "            if (PQresultStatus(res) != PGRES_COMMAND_OK) {\n";
      ss << "                std::string err = PQerrorMessage(conn);\n";
      ss << "                PQclear(res);\n";
      ss << "                return std::unexpected(err);\n";
      ss << "            }\n";
      ss << "            PQclear(res);\n";
      ss << "            return {};\n";
      ss << "        }\n";

      ss << "    };\n\n";
    }
  }

  ss << "}\n";
  return ss.str();
}

} // namespace cdd_cpp::orm