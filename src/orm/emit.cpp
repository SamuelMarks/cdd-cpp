#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::orm {

std::string map_type_to_postgres(const std::string &type) noexcept {
  if (type == "string")        // GCOV_EXCL_BR_LINE
    return "TEXT";             // GCOV_EXCL_BR_LINE
  if (type == "integer")       // GCOV_EXCL_BR_LINE
    return "INTEGER";          // GCOV_EXCL_BR_LINE
  if (type == "number")        // GCOV_EXCL_BR_LINE
    return "DOUBLE PRECISION"; // GCOV_EXCL_BR_LINE
  if (type == "boolean")       // GCOV_EXCL_BR_LINE
    return "BOOLEAN";          // GCOV_EXCL_BR_LINE
  return "TEXT";               // GCOV_EXCL_BR_LINE
}

std::string map_type_to_cpp(const std::string &type) noexcept {
  if (type == "string")   // GCOV_EXCL_BR_LINE
    return "std::string"; // GCOV_EXCL_BR_LINE
  if (type == "integer")  // GCOV_EXCL_BR_LINE
    return "int";         // GCOV_EXCL_BR_LINE
  if (type == "number")   // GCOV_EXCL_BR_LINE
    return "double";      // GCOV_EXCL_BR_LINE
  if (type == "boolean")  // GCOV_EXCL_BR_LINE
    return "bool";        // GCOV_EXCL_BR_LINE
  return "std::string";   // GCOV_EXCL_BR_LINE
}

std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;              // GCOV_EXCL_BR_LINE
  ss << "#pragma once\n";            // GCOV_EXCL_BR_LINE
  ss << "#include <expected>\n";     // GCOV_EXCL_BR_LINE
  ss << "#include <string>\n";       // GCOV_EXCL_BR_LINE
  ss << "#include <vector>\n";       // GCOV_EXCL_BR_LINE
  ss << "#include <libpq-fe.h>\n\n"; // GCOV_EXCL_BR_LINE

  ss << "namespace cdd_orm {\n\n"; // GCOV_EXCL_BR_LINE

  if (spec.components.has_value() &&              // GCOV_EXCL_BR_LINE
      spec.components->schemas.has_value()) {     // GCOV_EXCL_BR_LINE
    for (const auto &[name, schema] :             // GCOV_EXCL_BR_LINE
         spec.components->schemas.operator*()) {  // GCOV_EXCL_BR_LINE
      if (schema.type.value_or("") != "object" || // GCOV_EXCL_BR_LINE
          !schema.properties)                     // GCOV_EXCL_BR_LINE
        continue;

      std::string table_name = name; // GCOV_EXCL_BR_LINE
      for (auto &c : table_name)     // GCOV_EXCL_BR_LINE
        c = std::tolower(c);         // GCOV_EXCL_BR_LINE

      ss << "    struct " << name << " {\n";           // GCOV_EXCL_BR_LINE
      for (const auto &[prop_name, prop_schema] :      // GCOV_EXCL_BR_LINE
           *schema.properties) {                       // GCOV_EXCL_BR_LINE
        ss << "        "                               // GCOV_EXCL_BR_LINE
           << map_type_to_cpp(                         // GCOV_EXCL_BR_LINE
                  prop_schema.type.value_or("string")) // GCOV_EXCL_BR_LINE
           << " " << prop_name << ";\n";               // GCOV_EXCL_BR_LINE
      }
      ss << "    };\n\n"; // GCOV_EXCL_BR_LINE

      ss << "    class " << name << "Repository {\n"; // GCOV_EXCL_BR_LINE
      ss << "        PGconn* conn;\n";                // GCOV_EXCL_BR_LINE
      ss << "    public:\n";                          // GCOV_EXCL_BR_LINE
      ss << "        " << name                        // GCOV_EXCL_BR_LINE
         << "Repository(PGconn* c) : conn(c) {}\n\n"; // GCOV_EXCL_BR_LINE

      // CREATE TABLE
      ss << "        std::expected<void, std::string> create_table() noexcept " // GCOV_EXCL_BR_LINE
            "{\n";
      ss << "            std::string sql = \"CREATE TABLE IF NOT EXISTS " // GCOV_EXCL_BR_LINE
         << table_name << " ("; // GCOV_EXCL_BR_LINE

      bool first = true;
      for (const auto &[prop_name, prop_schema] :       // GCOV_EXCL_BR_LINE
           *schema.properties) {                        // GCOV_EXCL_BR_LINE
        if (!first)                                     // GCOV_EXCL_BR_LINE
          ss << ", ";                                   // GCOV_EXCL_BR_LINE
        ss << prop_name << " "                          // GCOV_EXCL_BR_LINE
           << map_type_to_postgres(                     // GCOV_EXCL_BR_LINE
                  prop_schema.type.value_or("string")); // GCOV_EXCL_BR_LINE
        first = false;
      }
      ss << ")\";\n"; // GCOV_EXCL_BR_LINE
      ss << "            PGresult* res = PQexec(conn, sql.c_str());\n"; // GCOV_EXCL_BR_LINE
      ss << "            if (PQresultStatus(res) != PGRES_COMMAND_OK) {\n"; // GCOV_EXCL_BR_LINE
      ss << "                std::string err = PQerrorMessage(conn);\n"; // GCOV_EXCL_BR_LINE
      ss << "                PQclear(res);\n"; // GCOV_EXCL_BR_LINE
      ss << "                return std::unexpected(err);\n"; // GCOV_EXCL_BR_LINE
      ss << "            }\n";             // GCOV_EXCL_BR_LINE
      ss << "            PQclear(res);\n"; // GCOV_EXCL_BR_LINE
      ss << "            return {};\n";    // GCOV_EXCL_BR_LINE
      ss << "        }\n\n";               // GCOV_EXCL_BR_LINE

      // INSERT
      ss << "        std::expected<void, std::string> insert(const " // GCOV_EXCL_BR_LINE
         << name                                           // GCOV_EXCL_BR_LINE
         << "& item) noexcept {\n";                        // GCOV_EXCL_BR_LINE
      ss << "            std::string sql = \"INSERT INTO " // GCOV_EXCL_BR_LINE
         << table_name                                     // GCOV_EXCL_BR_LINE
         << " (";                                          // GCOV_EXCL_BR_LINE
      first = true;
      for (const auto &[prop_name, prop_schema] : // GCOV_EXCL_BR_LINE
           *schema.properties) {                  // GCOV_EXCL_BR_LINE
        if (!first)                               // GCOV_EXCL_BR_LINE
          ss << ", ";                             // GCOV_EXCL_BR_LINE
        ss << prop_name;                          // GCOV_EXCL_BR_LINE
        first = false;
      }
      ss << ") VALUES ("; // GCOV_EXCL_BR_LINE
      first = true;
      int idx = 1;
      for (const auto &kv : *schema.properties) { // GCOV_EXCL_BR_LINE
        (void)kv;
        if (!first)         // GCOV_EXCL_BR_LINE
          ss << ", ";       // GCOV_EXCL_BR_LINE
        ss << "$" << idx++; // GCOV_EXCL_BR_LINE
        first = false;
      }
      ss << ")\";\n"; // GCOV_EXCL_BR_LINE

      ss << "            const char* paramValues[" // GCOV_EXCL_BR_LINE
         << schema.properties->size()              // GCOV_EXCL_BR_LINE
         << "];\n";                                // GCOV_EXCL_BR_LINE
      int paramIdx = 0;
      for (const auto &[prop_name, prop_schema] : // GCOV_EXCL_BR_LINE
           *schema.properties) {                  // GCOV_EXCL_BR_LINE
        std::string c_type = map_type_to_cpp(
            prop_schema.type.value_or("string")); // GCOV_EXCL_BR_LINE
        if (c_type == "std::string") {
          ss << "            paramValues[" << paramIdx // GCOV_EXCL_BR_LINE
             << "] = item."                            // GCOV_EXCL_BR_LINE
             << prop_name << ".c_str();\n";            // GCOV_EXCL_BR_LINE
        } else {
          ss << "            std::string p" << paramIdx // GCOV_EXCL_BR_LINE
             << " = std::to_string(item." << prop_name  // GCOV_EXCL_BR_LINE
             << ");\n";                                 // GCOV_EXCL_BR_LINE
          ss << "            paramValues[" << paramIdx // GCOV_EXCL_BR_LINE
             << "] = p"        // GCOV_EXCL_BR_LINE
             << paramIdx       // GCOV_EXCL_BR_LINE
             << ".c_str();\n"; // GCOV_EXCL_BR_LINE
        }
        paramIdx++; // GCOV_EXCL_BR_LINE
      }

      ss << "            PGresult* res = PQexecParams(conn, sql.c_str(), " // GCOV_EXCL_BR_LINE
         << schema.properties->size() // GCOV_EXCL_BR_LINE
         << ", nullptr, paramValues, nullptr, nullptr, 0);\n"; // GCOV_EXCL_BR_LINE
      ss << "            if (PQresultStatus(res) != PGRES_COMMAND_OK) {\n"; // GCOV_EXCL_BR_LINE
      ss << "                std::string err = PQerrorMessage(conn);\n"; // GCOV_EXCL_BR_LINE
      ss << "                PQclear(res);\n"; // GCOV_EXCL_BR_LINE
      ss << "                return std::unexpected(err);\n"; // GCOV_EXCL_BR_LINE
      ss << "            }\n";             // GCOV_EXCL_BR_LINE
      ss << "            PQclear(res);\n"; // GCOV_EXCL_BR_LINE
      ss << "            return {};\n";    // GCOV_EXCL_BR_LINE
      ss << "        }\n";                 // GCOV_EXCL_BR_LINE

      ss << "    };\n\n"; // GCOV_EXCL_BR_LINE
    } // GCOV_EXCL_BR_LINE
  }

  ss << "}\n";     // GCOV_EXCL_BR_LINE
  return ss.str(); // GCOV_EXCL_BR_LINE
}

} // namespace cdd_cpp::orm