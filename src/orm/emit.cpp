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

std::string emit(const openapi::OpenAPI & /*spec*/) noexcept {
  return ""; // Legacy
}

std::map<std::string, std::string> emit_modular(const openapi::OpenAPI &spec,
                                                bool with_postgres,
                                                bool /*with_faker*/) noexcept {
  std::map<std::string, std::string> files;

  // Error Enum
  std::stringstream err_ss;
  err_ss << "#pragma once\n#include <ostream>\n\nnamespace cdd_orm {\n";
  err_ss << "    enum class Error { DatabaseConnectionFailed, "
            "QueryExecutionFailed, NotFound, InvalidInput, NotImplemented };\n";
  err_ss << "    inline std::ostream& operator<<(std::ostream& os, const "
            "Error& e) {\n";
  err_ss << "        switch(e) {\n";
  err_ss << "            case Error::DatabaseConnectionFailed: return os << "
            "\"DatabaseConnectionFailed\";\n";
  err_ss << "            case Error::QueryExecutionFailed: return os << "
            "\"QueryExecutionFailed\";\n";
  err_ss << "            case Error::NotFound: return os << \"NotFound\";\n";
  err_ss << "            case Error::InvalidInput: return os << "
            "\"InvalidInput\";\n";
  err_ss << "            case Error::NotImplemented: return os << "
            "\"NotImplemented\";\n";
  err_ss << "            default: return os << \"Unknown Error\";\n";
  err_ss << "        }\n    }\n}\n";
  files["src/orm/error.hpp"] = err_ss.str();

  // Database Config
  if (with_postgres) {
    std::stringstream db_ss;
    db_ss << "#pragma once\n#include <string>\n#include <cstdlib>\n#include "
             "<expected>\n#include <vector>\n#include <libpq-fe.h>\n#include "
             "\"error.hpp\"\n\n";
    db_ss
        << "namespace cdd_orm {\n    /// Configuration required to connect to "
           "the underlying database.\n    struct DatabaseConfig {\n        "
           "std::string database_url;\n        /// If true, the connection "
           "factory will provision a temporary, throwaway schema/database.\n   "
           "     bool is_ephemeral;\n";
    db_ss << "        /// Instantiates a DatabaseConfig by reading environment "
             "variables (DATABASE_URL, EPHEMERAL_DB).\n        /// @return A "
             "populated DatabaseConfig object.\n        static DatabaseConfig "
             "from_env() noexcept {\n            "
             "DatabaseConfig config;\n";
    db_ss << "            if (const char* url = std::getenv(\"DATABASE_URL\")) "
             "config.database_url = url;\n";
    db_ss << "            if (const char* eph = std::getenv(\"EPHEMERAL_DB\")) "
             "config.is_ephemeral = (std::string(eph) == \"true\" || "
             "std::string(eph) == \"1\");\n";
    db_ss << "            else config.is_ephemeral = false;\n            "
             "return config;\n        }\n    };\n\n";
    db_ss << "    /// Factory responsible for establishing database "
             "connections based on configuration.\n    class ConnectionFactory "
             "{\n    public:\n        static "
             "std::expected<PGconn*, Error> create_connection(const "
             "DatabaseConfig& config) noexcept {\n";
    db_ss << "            if (config.database_url.empty()) return "
             "std::unexpected(Error::DatabaseConnectionFailed);\n";
    db_ss << "            PGconn* conn = "
             "PQconnectdb(config.database_url.c_str());\n";
    db_ss
        << "            if (PQstatus(conn) != CONNECTION_OK) { PQfinish(conn); "
           "return std::unexpected(Error::DatabaseConnectionFailed); }\n";
    db_ss << "            if (config.is_ephemeral) {\n                "
             "std::string schema_name = \"ephem_\" + "
             "std::to_string(std::rand());\n";
    db_ss << "                std::string sql = \"CREATE SCHEMA \" + "
             "schema_name + \"; SET search_path TO \" + schema_name + \";\";\n";
    db_ss << "                PGresult* res = PQexec(conn, sql.c_str());\n";
    db_ss << "                if (PQresultStatus(res) != PGRES_COMMAND_OK) { "
             "PQclear(res); PQfinish(conn); return "
             "std::unexpected(Error::QueryExecutionFailed); }\n";
    db_ss << "                PQclear(res);\n            }\n            return "
             "conn;\n        }\n    };\n}\n";
    files["src/orm/database.hpp"] = db_ss.str();
  }

  std::vector<std::string> names;
  if (spec.components.has_value() && spec.components->schemas.has_value()) {
    for (const auto &[name, schema] : spec.components->schemas.operator*()) {
      if (schema.type.value_or("") != "object" || !schema.properties)
        continue;
      names.push_back(name);
      std::string table_name = name;
      for (auto &c : table_name)
        c = std::tolower(c);

      std::stringstream ss;
      ss << "#pragma once\n#include <expected>\n#include <vector>\n#include "
            "\"error.hpp\"\n#include \"../models/"
         << name << ".hpp\"\n";
      if (with_postgres)
        ss << "#include <libpq-fe.h>\n";
      ss << "\nnamespace cdd_orm {\n";
      ss << "    class I" << name << "Dao {\n    public:\n        virtual ~I"
         << name << "Dao() = default;\n";
      ss << "        virtual std::expected<void, Error> create_table() "
            "noexcept = 0;\n";
      ss << "        virtual std::expected<void, Error> insert(const "
            "cdd_models::"
         << name
         << "& item) noexcept = 0;\n        virtual "
            "std::expected<std::vector<cdd_models::"
         << name << ">, Error> get_all() noexcept = 0;\n    };\n\n";
      ss << "    class Stub" << name << "Dao : public I" << name
         << "Dao {\n    public:\n";
      ss << "        std::expected<void, Error> create_table() noexcept "
            "override { return std::unexpected(Error::NotImplemented); }\n";
      ss << "        std::expected<void, Error> insert(const cdd_models::"
         << name
         << "&) noexcept override { return "
            "std::unexpected(Error::NotImplemented); }\n        "
            "std::expected<std::vector<cdd_models::"
         << name
         << ">, Error> get_all() noexcept override { return "
            "std::unexpected(Error::NotImplemented); }\n    };\n\n";
      if (with_postgres) {
        ss << "    class Concrete" << name << "Dao : public I" << name
           << "Dao {\n        PGconn* conn;\n    public:\n        Concrete"
           << name << "Dao(PGconn* c) : conn(c) {}\n";
        ss << "        std::expected<void, Error> create_table() noexcept "
              "override {\n";
        ss << "            std::string sql = \"CREATE TABLE IF NOT EXISTS "
           << table_name << " (\";\n";
        bool first = true;
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          if (!first)
            ss << "            sql += \", \";\n";
          ss << "            sql += \"" << prop_name << " "
             << map_type_to_postgres(prop_schema.type.value_or("string"))
             << "\";\n";
          first = false;
        }
        ss << "            sql += \")\";\n";
        ss << "            PGresult* res = PQexec(conn, sql.c_str());\n";
        ss << "            if (PQresultStatus(res) != PGRES_COMMAND_OK) { "
              "PQclear(res); return "
              "std::unexpected(Error::QueryExecutionFailed); }\n";
        ss << "            PQclear(res); return {};\n        }\n";
        ss << "        std::expected<void, Error> insert(const cdd_models::"
           << name << "& item) noexcept override {\n";
        ss << "            std::string sql = \"INSERT INTO " << table_name
           << " (\";\n";
        first = true;
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          if (!first)
            ss << "            sql += \", \";\n";
          ss << "            sql += \"" << prop_name << "\";\n";
          first = false;
        }
        ss << "            sql += \") VALUES (\";\n";
        first = true;
        int idx = 1;
        for (const auto &kv : *schema.properties) {
          (void)kv;
          if (!first)
            ss << "            sql += \", \";\n";
          ss << "            sql += \"$" << idx++ << "\";\n";
          first = false;
        }
        ss << "            sql += \")\";\n";
        ss << "            const char* paramValues["
           << schema.properties->size() << "];\n";
        int paramIdx = 0;
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          std::string c_type =
              map_type_to_cpp(prop_schema.type.value_or("string"));
          if (c_type == "std::string") {
            ss << "            paramValues[" << paramIdx << "] = item."
               << prop_name << " ? item." << prop_name << "->c_str() : \"\";\n";
          } else {
            ss << "            std::string p" << paramIdx
               << " = std::to_string(item." << prop_name << ".value_or(0));\n";
            ss << "            paramValues[" << paramIdx << "] = p" << paramIdx
               << ".c_str();\n";
          }
          paramIdx++;
        }
        ss << "            PGresult* res = PQexecParams(conn, sql.c_str(), "
           << schema.properties->size()
           << ", nullptr, paramValues, nullptr, nullptr, 0);\n";
        ss << "            if (PQresultStatus(res) != PGRES_COMMAND_OK) { "
              "PQclear(res); return "
              "std::unexpected(Error::QueryExecutionFailed); }\n";
        ss << "            PQclear(res); return {};\n        }\n        "
              "std::expected<std::vector<cdd_models::"
           << name
           << ">, Error> get_all() noexcept override {\n            "
              "std::vector<cdd_models::"
           << name
           << "> results;\n            std::string sql = \"SELECT * FROM \" + "
              "std::string(\""
           << table_name
           << "\");\n            PGresult* res = PQexec(conn, sql.c_str());\n  "
              "          if (PQresultStatus(res) != PGRES_TUPLES_OK) { "
              "PQclear(res); return "
              "std::unexpected(Error::QueryExecutionFailed); }\n            // "
              "In a real app we parse fields here.\n            "
              "PQclear(res);\n            return results;\n        }\n    };\n";
      }
      ss << "}\n";
      files["src/orm/" + name + "Dao.hpp"] = ss.str();
    }
  }

  std::stringstream rf_ss;
  rf_ss << "#pragma once\n#include <memory>\n";
  if (with_postgres)
    rf_ss << "#include <libpq-fe.h>\n";
  for (const auto &name : names)
    rf_ss << "#include \"" << name << "Dao.hpp\"\n";
  rf_ss << "\nnamespace cdd_orm {\n    /// Dependency Injection factory "
           "responsible for instantiating the correct Data Access Objects.\n   "
           " class RepositoryFactory {\n";
  if (with_postgres) {
    rf_ss << "        PGconn* conn;\n        bool is_stub;\n    public:\n      "
             "  RepositoryFactory(PGconn* c = nullptr, bool stub = false) : "
             "conn(c), is_stub(stub) {}\n";
  } else {
    rf_ss << "    public:\n        RepositoryFactory() {}\n";
  }
  for (const auto &name : names) {
    rf_ss << "        std::unique_ptr<I" << name << "Dao> create_" << name
          << "_dao() const noexcept {\n";
    if (with_postgres) {
      rf_ss << "            if (is_stub) return std::make_unique<Stub" << name
            << "Dao>();\n";
      rf_ss << "            return std::make_unique<Concrete" << name
            << "Dao>(conn);\n";
    } else {
      rf_ss << "            return std::make_unique<Stub" << name
            << "Dao>();\n";
    }
    rf_ss << "        }\n";
  }
  rf_ss << "    };\n}\n";
  files["src/orm/RepositoryFactory.hpp"] = rf_ss.str();

  return files;
}

} // namespace cdd_cpp::orm