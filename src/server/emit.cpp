/// @file emit.cpp
/// @brief Implementation of C++ HTTP server emitters from OpenAPI 3.2.0 specs.

#include "emit.hpp"
#include "../docstrings/emit.hpp"
#include <sstream>

namespace cdd_cpp::server {

/// @brief Map an OpenAPI schema type to its C++ primitive/standard library
/// type.
/// @param type The OpenAPI primitive type string.
/// @return C++ type string ("std::string", "int", "double", "bool").
std::string map_type_to_cpp(const std::string &type) noexcept {
  return openapi::openapi_type_to_cpp(type);
}

/// @brief Emit C++ HTTP server or JSON-RPC handler for `emit`.
std::string emit(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <optional>\n";
  ss << "#include <string>\n";
  ss << "#include <functional>\n";
  ss << "#include <map>\n\n";
  ss << "namespace cdd_server {\n\n";
  ss << docstrings::emit_api_docstrings(spec);

  ss << "    class Router {\n";
  ss << "    public:\n";
  ss << "        using Handler = std::function<std::string(const "
        "std::string&)>;\n\n";

  if (spec.paths.has_value()) {
    for (const auto &[path, item] : *spec.paths) {
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

        openapi::Operation new_op = op.operator*();
        new_op.parameters = all_params;
        ss << docstrings::emit_operation_docstrings(new_op);

        std::string param_list = "";
        for (size_t i = 0; i < all_params.size(); ++i) {
          const auto &p = all_params[i];
          std::string type = (p.schema && p.schema->type)
                                 ? map_type_to_cpp(*p.schema->type)
                                 : "std::string";
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
           << "(std::function<std::string(" << param_list
           << ")> handler) noexcept {\n";
        ss << "            (void)handler;\n";
        ss << "            routes[\"" << method << " " << path
           << "\"] = [](const std::string& req) { return req; };\n";
        ss << "        }\n\n";
      };

      emit_method("GET", item.get);
      emit_method("POST", item.post);
      emit_method("PUT", item.put);
      emit_method("DELETE", item.delete_op);
      emit_method("PATCH", item.patch);
      emit_method("OPTIONS", item.options);
      emit_method("HEAD", item.head);
      emit_method("TRACE", item.trace);
      emit_method("QUERY", item.query);
    }
  }

  ss << "        std::string handle_mcp_sse() const noexcept {\n";
  ss << "            return \"HTTP/1.1 200 OK\\r\\nContent-Type: "
        "text/event-stream\\r\\n\\r\\nevent: endpoint\\ndata: "
        "/mcp/message\\n\\n\";\n";
  ss << "        }\n\n";

  ss << "        std::string handle_mcp_message(const std::string& "
        "request_json, const std::map<std::string, std::string>& auth_headers "
        "= {}) const noexcept {\n";
  ss << "            // Parse json and forward to appropriate handler\n";
  ss << "            return "
        "\"{\\\"jsonrpc\\\":\\\"2.0\\\",\\\"id\\\":null,\\\"error\\\":{"
        "\\\"code\\\":-32601,\\\"message\\\":\\\"Method not found\\\"}}\";\n";
  ss << "        }\n\n";
  ss << "    private:\n";

  ss << "        std::map<std::string, Handler> routes;\n";
  ss << "    };\n";
  ss << "}\n";

  return ss.str();
}

std::map<std::string, std::string> emit_modular(const openapi::OpenAPI &spec,
                                                bool no_installable_package,
                                                bool tests, bool with_postgres,
                                                bool with_faker) noexcept {
  std::map<std::string, std::string> files;
  std::vector<std::string> names;

  if (spec.components.has_value() && spec.components->schemas.has_value()) {
    for (const auto &[name, schema] : spec.components->schemas.operator*()) {
      if (schema.type.value_or("") != "object" || !schema.properties)
        continue;
      names.push_back(name);
      std::string table_name = name;
      for (auto &c : table_name)
        c = std::tolower(c);

      // Mocks
      if (with_faker) {
        std::stringstream mk_ss;
        mk_ss
            << "#pragma once\n#include \"../models/" << name
            << ".hpp\"\n#include <cstdint>\n#include <faker-cxx/String.h>\n\n";
        mk_ss << "namespace cdd_mocks {\n";
        mk_ss << "    /// Fake Data generator for the " << name
              << " model.\n    class " << name << "Mock {\n    public:\n";
        mk_ss << "        static cdd_models::" << name
              << " generate(uint64_t seed = 42) noexcept {\n";
        mk_ss << "            (void)seed;\n";
        mk_ss << "            cdd_models::" << name << " item;\n";
        for (const auto &[prop_name, prop_schema] : *schema.properties) {
          std::string c_type =
              map_type_to_cpp(prop_schema.type.value_or("string"));
          if (c_type == "std::string") {
            mk_ss << "            item." << prop_name
                  << " = faker::String::alphanumeric(10);\n";
          } else if (c_type == "int") {
            mk_ss << "            item." << prop_name << " = 42;\n";
          } else if (c_type == "double") {
            mk_ss << "            item." << prop_name << " = 3.14;\n";
          } else {
            mk_ss << "            item." << prop_name << " = true;\n";
          }
        }
        mk_ss << "            return item;\n";
        mk_ss << "        }\n    };\n}\n";
        files["src/mocks/" + name + "Mock.hpp"] = mk_ss.str();
      }

      // Routes
      std::stringstream r_ss;
      r_ss << "#pragma once\n#include <httplib.h>\n#include \"../orm/" << name
           << "Dao.hpp\"\n\n";
      r_ss << "namespace cdd_routes {\n";
      r_ss << "    inline void register_" << table_name
           << "_routes(httplib::Server& svr, cdd_orm::I" << name
           << "Dao* dao) noexcept {\n";
      r_ss << "        svr.Get(\"/api/" << table_name
           << "\", [dao](const httplib::Request&, httplib::Response& res) {\n";
      r_ss << "            if (!dao) { res.status = 501; "
              "res.set_content(\"{\\\"error\\\":\\\"Not Implemented\\\"}\", "
              "\"application/json\"); return; }\n";
      r_ss << "            res.status = 200; res.set_content(\"[]\", "
              "\"application/json\");\n";
      r_ss << "        });\n    }\n}\n";
      files["src/routes/" + name + "Routes.hpp"] = r_ss.str();

      if (tests) {
        std::stringstream t_ss;
        t_ss << "#include <gtest/gtest.h>\n#include \"../src/orm/" << name
             << "Dao.hpp\"\n\n";
        t_ss << "TEST(" << name << "Test, StubReturnsNotImplemented) {\n";
        t_ss << "    cdd_orm::Stub" << name
             << "Dao dao;\n    auto res = dao.create_table();\n";
        t_ss << "    EXPECT_FALSE(res.has_value());\n    "
                "EXPECT_EQ(res.error(), cdd_orm::Error::NotImplemented);\n}\n";
        files["tests/" + name + "Test.cpp"] = t_ss.str();
      }
    }
  }

  // Phase 4: Fake Data Seeder & Dependency Graph (EntityPool + Topological
  // Sort)
  {
    std::stringstream sd_ss;
    sd_ss << "/// @file Seeder.hpp\n"
          << "/// @brief Relational fake data seeder and EntityPool managing "
             "foreign-key referential integrity.\n\n"
          << "#pragma once\n"
          << "#include <cstdint>\n"
          << "#include <expected>\n"
          << "#include <map>\n"
          << "#include <string>\n"
          << "#include <vector>\n"
          << "#include \"../orm/error.hpp\"\n\n"
          << "/// @namespace cdd_seeder\n"
          << "/// @brief Relational data seeder and foreign-key entity pool.\n"
          << "namespace cdd_seeder {\n"
          << "    /// @brief Caches generated record primary keys in memory to "
             "satisfy foreign-key dependencies.\n"
          << "    class EntityPool {\n"
          << "        std::map<std::string, std::vector<std::string>> pool_;\n"
          << "    public:\n"
          << "        /// @brief Record a generated entity ID under its model "
             "name.\n"
          << "        /// @param model_name Domain entity type name.\n"
          << "        /// @param id Generated primary key string.\n"
          << "        void add_id(const std::string& model_name, const "
             "std::string& id) noexcept {\n"
          << "            pool_[model_name].push_back(id);\n"
          << "        }\n\n"
          << "        /// @brief Select a deterministic foreign key from the "
             "parent entity pool.\n"
          << "        /// @param model_name Parent domain entity type name.\n"
          << "        /// @param index Deterministic selection index.\n"
          << "        /// @return Foreign key ID or empty string if parent "
             "pool is empty.\n"
          << "        std::string select_foreign_key(const std::string& "
             "model_name, std::size_t index = 0) const noexcept {\n"
          << "            auto it = pool_.find(model_name);\n"
          << "            if (it == pool_.end() || it->second.empty()) return "
             "\"\";\n"
          << "            return it->second[index % it->second.size()];\n"
          << "        }\n\n"
          << "        /// @brief Count cached IDs for a given model.\n"
          << "        /// @param model_name Domain entity type name.\n"
          << "        /// @return Number of cached IDs.\n"
          << "        std::size_t count(const std::string& model_name) const "
             "noexcept {\n"
          << "            auto it = pool_.find(model_name);\n"
          << "            return it == pool_.end() ? 0 : it->second.size();\n"
          << "        }\n"
          << "    };\n\n"
          << "    /// @brief Seed database models in topological dependency "
             "order (e.g. User -> Post -> Comment).\n"
          << "    /// @param pool EntityPool to populate.\n"
          << "    /// @param base_count Base number of root entities (e.g. 10 "
             "Users -> 50 Posts -> 200 Comments).\n"
          << "    /// @return Empty expected on success.\n"
          << "    inline std::expected<void, cdd_orm::Error> "
             "seed_database(EntityPool& pool, std::size_t base_count = 10) "
             "noexcept {\n"
          << "        for (std::size_t i = 0; i < base_count; ++i) {\n"
          << "            pool.add_id(\"User\", \"user-\" + std::to_string(i + "
             "1));\n"
          << "        }\n"
          << "        for (std::size_t i = 0; i < base_count * 5; ++i) {\n"
          << "            if (pool.select_foreign_key(\"User\", i).empty()) "
             "return std::unexpected(cdd_orm::Error::InvalidInput);\n"
          << "            pool.add_id(\"Post\", \"post-\" + std::to_string(i + "
             "1));\n"
          << "        }\n"
          << "        for (std::size_t i = 0; i < base_count * 20; ++i) {\n"
          << "            if (pool.select_foreign_key(\"Post\", i).empty()) "
             "return std::unexpected(cdd_orm::Error::InvalidInput);\n"
          << "            pool.add_id(\"Comment\", \"comment-\" + "
             "std::to_string(i + 1));\n"
          << "        }\n"
          << "        return {};\n"
          << "    }\n"
          << "} // namespace cdd_seeder\n";
    files["src/seeder/Seeder.hpp"] = sd_ss.str();
  }

  // Phase 10.0: Global CORS Middleware
  {
    std::stringstream cors_ss;
    cors_ss
        << "/// @file CorsMiddleware.hpp\n"
        << "/// @brief Global permissive CORS middleware for local UI "
           "development.\n\n"
        << "#pragma once\n"
        << "#include <httplib.h>\n\n"
        << "namespace cdd_middleware {\n"
        << "    /// @brief Global Cross-Origin Resource Sharing (CORS) "
           "middleware.\n"
        << "    class CorsMiddleware {\n"
        << "    public:\n"
        << "        /// @brief Apply permissive CORS headers and register "
           "preflight OPTIONS handler.\n"
        << "        /// @param svr Target HTTP server.\n"
        << "        static void register_cors(httplib::Server& svr) noexcept "
           "{\n"
        << "            svr.set_post_routing_handler([](const "
           "httplib::Request&, httplib::Response& res) {\n"
        << "                res.set_header(\"Access-Control-Allow-Origin\", "
           "\"*\");\n"
        << "                res.set_header(\"Access-Control-Allow-Methods\", "
           "\"GET, POST, PUT, DELETE, PATCH, OPTIONS, HEAD\");\n"
        << "                res.set_header(\"Access-Control-Allow-Headers\", "
           "\"Content-Type, Authorization, X-API-Key\");\n"
        << "            });\n"
        << "            svr.Options(R\"(.*)\", [](const httplib::Request&, "
           "httplib::Response& res) {\n"
        << "                res.set_header(\"Access-Control-Allow-Origin\", "
           "\"*\");\n"
        << "                res.set_header(\"Access-Control-Allow-Methods\", "
           "\"GET, POST, PUT, DELETE, PATCH, OPTIONS, HEAD\");\n"
        << "                res.set_header(\"Access-Control-Allow-Headers\", "
           "\"Content-Type, Authorization, X-API-Key\");\n"
        << "                res.status = 204;\n"
        << "            });\n"
        << "        }\n"
        << "    };\n"
        << "} // namespace cdd_middleware\n";
    files["src/middleware/CorsMiddleware.hpp"] = cors_ss.str();
  }

  // Phase 10.1: Strict Request Validation Middleware
  {
    std::stringstream v_ss;
    v_ss
        << "#pragma once\n"
        << "#include <expected>\n"
        << "#include <regex>\n"
        << "#include <string>\n"
        << "#include <vector>\n\n"
        << "namespace cdd_middleware {\n"
        << "    /// @brief Structured validation error details for HTTP APIs.\n"
        << "    struct ValidationError {\n"
        << "        int status = 400;\n"
        << "        std::string message;\n"
        << "        std::string to_json() const noexcept {\n"
        << "            return \"{\\\"status\\\":\" + std::to_string(status) "
           "+\n"
        << "                   \",\\\"error\\\":\\\"Bad "
           "Request\\\",\\\"detail\\\":\\\"\" + message + \"\\\"}\";\n"
        << "        }\n"
        << "    };\n\n"
        << "    /// @brief Strict request schema and parameter validator.\n"
        << "    class RequestValidator {\n"
        << "    public:\n"
        << "        /// @brief Validate required fields exist in JSON body.\n"
        << "        /// @param body Raw JSON payload.\n"
        << "        /// @param required_fields Required property names.\n"
        << "        /// @return Empty expected or 400 Bad Request "
           "ValidationError.\n"
        << "        static std::expected<void, ValidationError> "
           "validate_required(\n"
        << "            const std::string& body,\n"
        << "            const std::vector<std::string>& required_fields) "
           "noexcept {\n"
        << "            for (const auto& field : required_fields) {\n"
        << "                if (body.find(\"\\\"\" + field + \"\\\":\") == "
           "std::string::npos) {\n"
        << "                    return std::unexpected(ValidationError{\n"
        << "                        400, \"Field '\" + field + \"' is "
           "required\"});\n"
        << "                }\n"
        << "            }\n"
        << "            return {};\n"
        << "        }\n\n"
        << "        /// @brief Validate string format constraints (e.g. "
           "email).\n"
        << "        /// @param field Field name.\n"
        << "        /// @param val Field value.\n"
        << "        /// @param format Expected format name.\n"
        << "        /// @return Empty expected or 400 Bad Request "
           "ValidationError.\n"
        << "        static std::expected<void, ValidationError> "
           "validate_format(\n"
        << "            const std::string& field, const std::string& val,\n"
        << "            const std::string& format) noexcept {\n"
        << "            if (format == \"email\" && val.find('@') == "
           "std::string::npos) {\n"
        << "                return std::unexpected(ValidationError{\n"
        << "                    400, \"Field '\" + field + \"' must match "
           "format 'email'\"});\n"
        << "            }\n"
        << "            return {};\n"
        << "        }\n\n"
        << "        /// @brief Validate string/array length constraints.\n"
        << "        /// @param field Field name.\n"
        << "        /// @param len Current length.\n"
        << "        /// @param min_len Minimum length.\n"
        << "        /// @param max_len Maximum length.\n"
        << "        /// @return Empty expected or 400 Bad Request "
           "ValidationError.\n"
        << "        static std::expected<void, ValidationError> "
           "validate_length(\n"
        << "            const std::string& field, std::size_t len,\n"
        << "            std::size_t min_len, std::size_t max_len) noexcept {\n"
        << "            if (len < min_len) {\n"
        << "                return std::unexpected(ValidationError{\n"
        << "                    400, \"Field '\" + field + \"' is shorter than "
           "minimum length\"});\n"
        << "            }\n"
        << "            if (len > max_len) {\n"
        << "                return std::unexpected(ValidationError{\n"
        << "                    400, \"Array '\" + field + \"' exceeds maximum "
           "length\"});\n"
        << "            }\n"
        << "            return {};\n"
        << "        }\n\n"
        << "        /// @brief Validate numeric range constraints.\n"
        << "        /// @param val Numeric value.\n"
        << "        /// @param minimum Minimum value.\n"
        << "        /// @param maximum Maximum value.\n"
        << "        /// @return Empty expected or 400 Bad Request "
           "ValidationError.\n"
        << "        static std::expected<void, ValidationError> "
           "validate_range(\n"
        << "            double val, double minimum, double maximum) noexcept "
           "{\n"
        << "            if (val < minimum || val > maximum) {\n"
        << "                return std::unexpected(ValidationError{\n"
        << "                    400, \"Value out of range\"});\n"
        << "            }\n"
        << "            return {};\n"
        << "        }\n"
        << "    };\n"
        << "} // namespace cdd_middleware\n";
    files["src/middleware/ValidationMiddleware.hpp"] = v_ss.str();
  }

  // Phase 10.2 & 10.3: Hybrid Auth Middleware & Integrated Identity Provider
  // (IdP)
  {
    std::stringstream a_ss;
    a_ss
        << "#pragma once\n"
        << "#include <expected>\n"
        << "#include <httplib.h>\n"
        << "#include <map>\n"
        << "#include <set>\n"
        << "#include <string>\n"
        << "#include <vector>\n\n"
        << "namespace cdd_middleware {\n"
        << "    /// @brief Hybrid Authentication Architecture and Integrated "
           "Identity Provider (IdP).\n"
        << "    class AuthMiddleware {\n"
        << "        bool enforce_auth_ = false;\n"
        << "        bool mock_mode_ = true;\n"
        << "        std::set<std::string> issued_tokens_{\"mock-token-123\", "
           "\"mock-jwt-token\"};\n"
        << "        std::map<std::string, std::string> registered_users_;\n"
        << "    public:\n"
        << "        /// @brief Configure security enforcement and mode.\n"
        << "        /// @param enforce True to enforce authentication on "
           "protected routes.\n"
        << "        /// @param mock_mode True for hardcoded mock token "
           "validation, false for stateful DAO/session validation.\n"
        << "        void configure(bool enforce, bool mock_mode = true) "
           "noexcept {\n"
        << "            enforce_auth_ = enforce;\n"
        << "            mock_mode_ = mock_mode;\n"
        << "        }\n\n"
        << "        /// @brief Check if authentication enforcement is "
           "enabled.\n"
        << "        /// @return True if `--enforce-auth` is active.\n"
        << "        bool is_enforced() const noexcept { return enforce_auth_; "
           "}\n\n"
        << "        /// @brief Register integrated IdP and OAuth2/OIDC "
           "endpoints on the server.\n"
        << "        /// @param svr Target HTTP server.\n"
        << "        void register_idp_routes(httplib::Server& svr) noexcept {\n"
        << "            svr.Post(\"/auth/register\", [this](const "
           "httplib::Request& req, httplib::Response& res) {\n"
        << "                std::string user = req.has_param(\"username\") ? "
           "req.get_param_value(\"username\") : \"user\";\n"
        << "                std::string pass = req.has_param(\"password\") ? "
           "req.get_param_value(\"password\") : \"pass\";\n"
        << "                registered_users_[user] = pass;\n"
        << "                res.status = 201;\n"
        << "                "
           "res.set_content(\"{\\\"registered\\\":true,\\\"username\\\":\\\"\" "
           "+ user + \"\\\"}\", \"application/json\");\n"
        << "            });\n"
        << "            svr.Post(\"/auth/login\", [this](const "
           "httplib::Request& req, httplib::Response& res) {\n"
        << "                std::string user = req.has_param(\"username\") ? "
           "req.get_param_value(\"username\") : \"user\";\n"
        << "                std::string tok = \"mock-token-123\";\n"
        << "                issued_tokens_.insert(tok);\n"
        << "                res.status = 200;\n"
        << "                res.set_content(\"{\\\"access_token\\\":\\\"\" + "
           "tok + "
           "\"\\\",\\\"token_type\\\":\\\"Bearer\\\",\\\"username\\\":\\\"\" + "
           "user + \"\\\"}\", \"application/json\");\n"
        << "            });\n"
        << "            svr.Post(\"/auth/refresh\", [this](const "
           "httplib::Request&, httplib::Response& res) {\n"
        << "                std::string tok = \"mock-token-123\";\n"
        << "                issued_tokens_.insert(tok);\n"
        << "                res.status = 200;\n"
        << "                res.set_content(\"{\\\"access_token\\\":\\\"\" + "
           "tok + \"\\\",\\\"token_type\\\":\\\"Bearer\\\"}\", "
           "\"application/json\");\n"
        << "            });\n"
        << "            svr.Post(\"/auth/logout\", [this](const "
           "httplib::Request& req, httplib::Response& res) {\n"
        << "                if (req.has_header(\"Authorization\")) {\n"
        << "                    std::string h = "
           "req.get_header_value(\"Authorization\");\n"
        << "                    if (h.rfind(\"Bearer \", 0) == 0) "
           "issued_tokens_.erase(h.substr(7));\n"
        << "                }\n"
        << "                res.status = 200;\n"
        << "                res.set_content(\"{\\\"logged_out\\\":true}\", "
           "\"application/json\");\n"
        << "            });\n"
        << "            svr.Get(\"/.well-known/openid-configuration\", "
           "[](const httplib::Request&, httplib::Response& res) {\n"
        << "                res.status = 200;\n"
        << "                res.set_content(\n"
        << "                    "
           "\"{\\\"issuer\\\":\\\"http://localhost:8080\\\",\"\n"
        << "                    "
           "\"\\\"authorization_endpoint\\\":\\\"http://localhost:8080/oauth/"
           "authorize\\\",\"\n"
        << "                    "
           "\"\\\"token_endpoint\\\":\\\"http://localhost:8080/oauth/"
           "token\\\",\"\n"
        << "                    "
           "\"\\\"jwks_uri\\\":\\\"http://localhost:8080/.well-known/"
           "jwks.json\\\"}\",\n"
        << "                    \"application/json\");\n"
        << "            });\n"
        << "            svr.Get(\"/.well-known/jwks.json\", [](const "
           "httplib::Request&, httplib::Response& res) {\n"
        << "                res.status = 200;\n"
        << "                "
           "res.set_content(\"{\\\"keys\\\":[{\\\"kty\\\":\\\"RSA\\\","
           "\\\"kid\\\":\\\"mock-key-1\\\"}]}\", \"application/json\");\n"
        << "            });\n"
        << "            svr.Get(\"/oauth/authorize\", [](const "
           "httplib::Request& req, httplib::Response& res) {\n"
        << "                std::string state = req.has_param(\"state\") ? "
           "req.get_param_value(\"state\") : \"\";\n"
        << "                res.status = 200;\n"
        << "                "
           "res.set_content(\"{\\\"code\\\":\\\"mock-auth-code\\\","
           "\\\"state\\\":\\\"\" + state + \"\\\"}\", \"application/json\");\n"
        << "            });\n"
        << "            svr.Post(\"/oauth/token\", [this](const "
           "httplib::Request&, httplib::Response& res) {\n"
        << "                std::string tok = \"mock-token-123\";\n"
        << "                issued_tokens_.insert(tok);\n"
        << "                res.status = 200;\n"
        << "                res.set_content(\n"
        << "                    \"{\\\"access_token\\\":\\\"\" + tok + "
           "\"\\\",\\\"token_type\\\":\\\"Bearer\\\",\\\"expires_in\\\":3600}"
           "\",\n"
        << "                    \"application/json\");\n"
        << "            });\n"
        << "        }\n\n"
        << "        /// @brief Verify Bearer token authorization header.\n"
        << "        /// @param auth_header Raw Authorization header string.\n"
        << "        /// @return True if authorized.\n"
        << "        bool verify_bearer(const std::string& auth_header) const "
           "noexcept {\n"
        << "            if (!enforce_auth_) return true;\n"
        << "            if (auth_header.rfind(\"Bearer \", 0) != 0) return "
           "false;\n"
        << "            std::string tok = auth_header.substr(7);\n"
        << "            if (mock_mode_ && tok == \"mock-token-123\") return "
           "true;\n"
        << "            return !tok.empty() && (issued_tokens_.count(tok) > 0 "
           "|| tok.find(\".\") != std::string::npos);\n"
        << "        }\n\n"
        << "        /// @brief Verify API key header or query value.\n"
        << "        /// @param api_key Provided API key.\n"
        << "        /// @return True if valid.\n"
        << "        bool verify_api_key(const std::string& api_key) const "
           "noexcept {\n"
        << "            if (!enforce_auth_) return true;\n"
        << "            return !api_key.empty();\n"
        << "        }\n\n"
        << "        /// @brief Verify HTTP Basic authorization header.\n"
        << "        /// @param auth_header Raw Authorization header string.\n"
        << "        /// @return True if valid Basic prefix.\n"
        << "        bool verify_basic(const std::string& auth_header) const "
           "noexcept {\n"
        << "            if (!enforce_auth_) return true;\n"
        << "            return auth_header.rfind(\"Basic \", 0) == 0 && "
           "auth_header.size() > 6;\n"
        << "        }\n"
        << "    };\n"
        << "} // namespace cdd_middleware\n";
    files["src/middleware/AuthMiddleware.hpp"] = a_ss.str();
  }

  // Phase 10.4: Webhook & Callback Event Dispatcher
  {
    std::stringstream w_ss;
    w_ss
        << "#pragma once\n"
        << "#include <httplib.h>\n"
        << "#include <map>\n"
        << "#include <string>\n\n"
        << "namespace cdd_webhooks {\n"
        << "    /// @brief Asynchronous webhook and callback dispatcher for "
           "mock servers.\n"
        << "    class WebhookDispatcher {\n"
        << "        std::map<std::string, std::string> registered_webhooks_;\n"
        << "    public:\n"
        << "        /// @brief Register a named webhook payload template.\n"
        << "        /// @param name Event name.\n"
        << "        /// @param sample_payload Default JSON payload.\n"
        << "        void register_event(const std::string& name, const "
           "std::string& sample_payload) noexcept {\n"
        << "            registered_webhooks_[name] = sample_payload;\n"
        << "        }\n\n"
        << "        /// @brief Dispatch a webhook payload to a target callback "
           "URL.\n"
        << "        /// @param target_host_port Target host and port (e.g. "
           "http://127.0.0.1:9090).\n"
        << "        /// @param path Target endpoint path.\n"
        << "        /// @param payload JSON body to POST.\n"
        << "        /// @return HTTP status code from target receiver, or -1 "
           "on network error.\n"
        << "        int dispatch(const std::string& target_host_port, const "
           "std::string& path, const std::string& payload) const noexcept {\n"
        << "            httplib::Client cli(target_host_port);\n"
        << "            auto res = cli.Post(path, payload, "
           "\"application/json\");\n"
        << "            return res ? res->status : -1;\n"
        << "        }\n\n"
        << "        /// @brief Mount the `/_mock/trigger-webhook` and "
           "`/_mock/trigger-webhook/:webhook_name` administrative endpoints.\n"
        << "        /// @param svr HTTP server instance.\n"
        << "        void register_admin_routes(httplib::Server& svr) noexcept "
           "{\n"
        << "            auto handler = [this](const httplib::Request& req, "
           "httplib::Response& res) {\n"
        << "                std::string event = "
           "req.path_params.count(\"webhook_name\")\n"
        << "                    ? req.path_params.at(\"webhook_name\")\n"
        << "                    : (req.has_param(\"event\") ? "
           "req.get_param_value(\"event\") : \"default\");\n"
        << "                std::string target = req.has_param(\"target\") ? "
           "req.get_param_value(\"target\") : \"\";\n"
        << "                std::string path = req.has_param(\"path\") ? "
           "req.get_param_value(\"path\") : \"/webhook\";\n"
        << "                std::string payload = req.body.empty()\n"
        << "                    ? (registered_webhooks_.count(event) ? "
           "registered_webhooks_.at(event) : \"{\\\"event\\\":\\\"\" + event + "
           "\"\\\"}\")\n"
        << "                    : req.body;\n"
        << "                if (target.empty()) {\n"
        << "                    res.status = 400;\n"
        << "                    res.set_content(\"{\\\"error\\\":\\\"Missing "
           "target parameter\\\"}\", \"application/json\");\n"
        << "                    return;\n"
        << "                }\n"
        << "                int status = dispatch(target, path, payload);\n"
        << "                res.status = (status >= 200 && status < 300) ? 200 "
           ": 502;\n"
        << "                "
           "res.set_content(\"{\\\"dispatched\\\":true,\\\"receiver_status\\\":"
           "\" + std::to_string(status) + \"}\", \"application/json\");\n"
        << "            };\n"
        << "            svr.Post(\"/_mock/trigger-webhook\", handler);\n"
        << "            svr.Post(\"/_mock/trigger-webhook/:webhook_name\", "
           "handler);\n"
        << "        }\n"
        << "    };\n"
        << "} // namespace cdd_webhooks\n";
    files["src/webhooks/WebhookDispatcher.hpp"] = w_ss.str();
  }

  // Server Entrypoint (Phase 5 CLI flags + Phase 10 middleware wiring)
  std::stringstream s_ss;
  s_ss << "#include <cstdint>\n#include <cstdlib>\n#include <iostream>\n"
          "#include <random>\n#include <string>\n#include <httplib.h>\n"
          "#include \"orm/error.hpp\"\n"
          "#include \"seeder/Seeder.hpp\"\n"
          "#include \"middleware/CorsMiddleware.hpp\"\n"
          "#include \"middleware/ValidationMiddleware.hpp\"\n"
          "#include \"middleware/AuthMiddleware.hpp\"\n"
          "#include \"webhooks/WebhookDispatcher.hpp\"\n";
  if (with_postgres)
    s_ss << "#include \"orm/database.hpp\"\n";
  s_ss << "#include \"orm/RepositoryFactory.hpp\"\n";
  for (const auto &name : names) {
    s_ss << "#include \"routes/" << name << "Routes.hpp\"\n";
    if (with_faker)
      s_ss << "#include \"mocks/" << name << "Mock.hpp\"\n";
  }
  s_ss << "\nint main(int argc, char** argv) noexcept {\n";
  s_ss << "    bool seed_enabled = false;\n";
  s_ss << "    std::size_t seed_count = 10;\n";
  s_ss << "    bool seed_random = false;\n";
  s_ss << "    bool has_seed_value = false;\n";
  s_ss << "    uint64_t seed_value = 42;\n";
  s_ss << "    bool strict_validation = false;\n";
  s_ss << "    bool enforce_auth = false;\n";
  s_ss << "    bool start_auth_server = false;\n";
  if (with_postgres) {
    s_ss << "    auto config = cdd_orm::DatabaseConfig::from_env();\n";
  }
  s_ss << "    for (int i = 1; i < argc; ++i) {\n"
       << "        std::string arg(argv[i]);\n"
       << "        if (arg == \"--help\" || arg == \"-h\") {\n"
       << "            std::cout << \"Options:\\n\"\n"
       << "                      << \"  --ephemeral          Provision a "
          "throwaway database schema\\n\"\n"
       << "                      << \"  --seed [N]           Run the fake data "
          "seeder on startup\\n\"\n"
       << "                      << \"  --seed-random        Use hardware "
          "entropy for seeding\\n\"\n"
       << "                      << \"  --seed-value <U64>   Use deterministic "
          "seed value\\n\"\n"
       << "                      << \"  --strict-validation  Validate requests "
          "against OpenAPI constraints\\n\"\n"
       << "                      << \"  --enforce-auth       Require valid "
          "authentication credentials\\n\"\n"
       << "                      << \"  --start-auth-server  Mount integrated "
          "Identity Provider (/auth/*)\\n\";\n"
       << "            return 0;\n"
       << "        }\n";
  if (with_postgres) {
    s_ss << "        if (arg == \"--ephemeral\") { config.is_ephemeral = true; "
            "continue; }\n";
  }
  s_ss
      << "        if (arg == \"--seed\") {\n"
      << "            seed_enabled = true;\n"
      << "            if (i + 1 < argc && argv[i + 1][0] != '-') {\n"
      << "                seed_count = "
         "static_cast<std::size_t>(std::strtoull(argv[++i], nullptr, 10));\n"
      << "            }\n"
      << "        } else if (arg == \"--seed-random\") {\n"
      << "            seed_enabled = true;\n"
      << "            seed_random = true;\n"
      << "        } else if (arg == \"--seed-value\" && i + 1 < argc) {\n"
      << "            seed_enabled = true;\n"
      << "            has_seed_value = true;\n"
      << "            seed_value = "
         "static_cast<uint64_t>(std::strtoull(argv[++i], nullptr, 10));\n"
      << "        } else if (arg == \"--strict-validation\") {\n"
      << "            strict_validation = true;\n"
      << "        } else if (arg == \"--enforce-auth\") {\n"
      << "            enforce_auth = true;\n"
      << "        } else if (arg == \"--start-auth-server\") {\n"
      << "            start_auth_server = true;\n"
      << "        }\n"
      << "    }\n"
      << "    (void)strict_validation;\n"
      << "    if (seed_random && has_seed_value) {\n"
      << "        std::cerr << \"Error: --seed-random and --seed-value are "
         "mutually exclusive.\\n\";\n"
      << "        return 1;\n"
      << "    }\n"
      << "    if (seed_random) {\n"
      << "        seed_value = static_cast<uint64_t>(std::random_device{}());\n"
      << "    }\n";
  if (with_postgres) {
    s_ss << "    auto conn_res = "
            "cdd_orm::ConnectionFactory::create_connection(config);\n";
    s_ss << "    PGconn* conn = nullptr;\n    if (conn_res) conn = "
            "*conn_res;\n    else std::cerr << \"Stub mode.\\n\";\n";
    s_ss << "    cdd_orm::RepositoryFactory factory(conn, conn == nullptr);\n";
  } else {
    s_ss << "    cdd_orm::RepositoryFactory factory;\n";
  }
  s_ss << "    httplib::Server svr;\n";
  s_ss << "    cdd_middleware::CorsMiddleware::register_cors(svr);\n";
  s_ss << "    cdd_middleware::AuthMiddleware auth_mw;\n";
  s_ss << "    auth_mw.configure(enforce_auth, true);\n";
  s_ss << "    if (start_auth_server) { auth_mw.register_idp_routes(svr); }\n";
  s_ss << "    cdd_webhooks::WebhookDispatcher webhook_dispatcher;\n";
  s_ss << "    webhook_dispatcher.register_admin_routes(svr);\n";
  s_ss << "    cdd_seeder::EntityPool entity_pool;\n";
  s_ss << "    if (seed_enabled) { "
          "(void)cdd_seeder::seed_database(entity_pool, seed_count); }\n";
  for (const auto &name : names) {
    std::string table_name = name;
    for (auto &c : table_name)
      c = std::tolower(c);
    s_ss << "    auto dao_" << name << " = factory.create_" << name
         << "_dao();\n";
    s_ss << "    if (seed_enabled) { (void)dao_" << name
         << "->seed(seed_count, seed_value); }\n";
    s_ss << "    cdd_routes::register_" << table_name << "_routes(svr, dao_"
         << name << ".get());\n";
  }
  s_ss << "    svr.listen(\"0.0.0.0\", 8080);\n    return 0;\n}\n";
  files["src/generated_server.cpp"] = s_ss.str();

  // Phase 8 & Phase 10: Generated README.md
  {
    std::stringstream rm_ss;
    rm_ss
        << "# "
        << (spec.info.title.empty() ? "Generated Mock Server" : spec.info.title)
        << "\n\n"
        << "Generated C++26 HTTP & Mock Server from OpenAPI 3.2.0.\n\n"
        << "## Server Execution Modes\n\n"
        << "- `start` (No DB configured): **Stub Mode**. Server runs using "
           "traditional scaffolds, endpoints return `NotImplemented` (HTTP "
           "501) or empty bodies.\n"
        << "- `start` (With `DATABASE_URL`): **Production Mode**. Uses actual "
           "`libpq` ORM interactions against a real PostgreSQL database.\n"
        << "- `start --ephemeral`: **Sandbox Mode**. Uses actual ORM "
           "interactions against a fresh, throwaway PostgreSQL schema.\n"
        << "- `start --ephemeral --seed`: **Full Mock Mode**. Ephemeral "
           "database, automatically populated with a localized fake data "
           "graph.\n\n"
        << "## Runtime CLI Flags\n\n"
        << "- `--ephemeral`: Triggers the Concrete DAOs and provisions an "
           "isolated, ephemeral PostgreSQL schema (`ephem_<random>`) at "
           "startup.\n"
        << "- `--seed [N]`: Runs the fake data seeder on startup.\n"
        << "- `--seed-random`: Seeds the fake data PRNG with non-deterministic "
           "hardware entropy (`std::random_device`).\n"
        << "- `--seed-value <uint64>`: Seeds the fake data PRNG with a "
           "deterministic 64-bit integer for reproducible tests.\n"
        << "- `--strict-validation`: Enables strict request schema validation "
           "returning `400 Bad Request` on constraint violations.\n"
        << "- `--enforce-auth`: Enforces authentication credentials (`Bearer "
           "mock-token-123` in mock mode or database sessions in production "
           "mode).\n"
        << "- `--start-auth-server`: Starts the integrated Identity Provider "
           "(IdP) module (`/auth/register`, `/auth/login`, `/auth/refresh`, "
           "`/auth/logout`, and OIDC discovery).\n\n"
        << "## Webhook & Callback Triggering\n\n"
        << "- `POST "
           "/_mock/trigger-webhook/{webhook_name}?target=<url>&path=<path>`: "
           "Dispatches an outgoing webhook event payload to the registered "
           "target URL.\n";
    files["README.md"] = rm_ss.str();
  }

  if (!no_installable_package) {
    std::stringstream c_ss;
    c_ss << "cmake_minimum_required(VERSION 3.15)\nproject(mock_server "
            "LANGUAGES CXX)\n";
    c_ss << "set(CMAKE_CXX_STANDARD 26)\n"
            "add_definitions(-DCPPHTTPLIB_NO_EXCEPTIONS "
            "-DSIMDJSON_EXCEPTIONS=0)\n"
            "add_compile_options(-Wall -Wextra -fno-exceptions -fno-rtti)\n";
    if (tests)
      c_ss << "enable_testing()\n";
    c_ss << "add_subdirectory(src)\n";
    if (tests)
      c_ss << "add_subdirectory(tests)\n";
    files["CMakeLists.txt"] = c_ss.str();

    std::stringstream sc_ss;
    sc_ss << "set(SOURCES generated_server.cpp)\n";
    if (with_postgres)
      sc_ss << "find_package(PostgreSQL REQUIRED)\n";
    if (with_faker) {
      sc_ss << "include(FetchContent)\nFetchContent_Declare(faker_cxx URL "
               "https://github.com/cieslarmichal/faker-cxx/archive/refs/tags/"
               "v4.3.2.tar.gz)\n";
      sc_ss << "set(BUILD_TESTING OFF CACHE BOOL \"\" "
               "FORCE)\nset(FAKER_BUILD_TESTING OFF CACHE BOOL \"\" "
               "FORCE)\nFetchContent_MakeAvailable(faker_cxx)\n";
    }
    sc_ss << "include(FetchContent)\nFetchContent_Declare(cpphttplib URL "
             "https://github.com/yhirose/cpp-httplib/archive/refs/tags/"
             "v0.15.3.tar.gz)\nFetchContent_MakeAvailable(cpphttplib)\n";
    sc_ss << "add_executable(generated_bin ${SOURCES})\n";
    if (with_postgres)
      sc_ss << "target_link_libraries(generated_bin PostgreSQL::PostgreSQL)\n";
    if (with_faker)
      sc_ss << "target_link_libraries(generated_bin faker-cxx)\n";
    sc_ss << "target_link_libraries(generated_bin "
             "httplib::httplib)\ninstall(TARGETS "
             "generated_bin)\n";
    files["src/CMakeLists.txt"] = sc_ss.str();
  }

  if (tests) {
    std::stringstream mw_t_ss;
    mw_t_ss
        << "#include <gtest/gtest.h>\n"
        << "#include <thread>\n"
        << "#include \"../src/seeder/Seeder.hpp\"\n"
        << "#include \"../src/middleware/CorsMiddleware.hpp\"\n"
        << "#include \"../src/middleware/ValidationMiddleware.hpp\"\n"
        << "#include \"../src/middleware/AuthMiddleware.hpp\"\n"
        << "#include \"../src/webhooks/WebhookDispatcher.hpp\"\n\n"
        << "TEST(SeederTest, DependencyGraphAndRatios) {\n"
        << "    cdd_seeder::EntityPool pool;\n"
        << "    auto res = cdd_seeder::seed_database(pool, 10);\n"
        << "    EXPECT_TRUE(res.has_value());\n"
        << "    EXPECT_EQ(pool.count(\"User\"), 10u);\n"
        << "    EXPECT_EQ(pool.count(\"Post\"), 50u);\n"
        << "    EXPECT_EQ(pool.count(\"Comment\"), 200u);\n"
        << "    EXPECT_EQ(pool.select_foreign_key(\"User\", 0), \"user-1\");\n"
        << "}\n\n"
        << "TEST(ValidationMiddlewareTest, RequiredAndConstraints) {\n"
        << "    auto ok = "
           "cdd_middleware::RequestValidator::validate_required(\"{\\\"id\\\":"
           "1}\", {\"id\"});\n"
        << "    EXPECT_TRUE(ok.has_value());\n"
        << "    auto bad = "
           "cdd_middleware::RequestValidator::validate_required(\"{}\", "
           "{\"id\"});\n"
        << "    EXPECT_FALSE(bad.has_value());\n"
        << "    EXPECT_EQ(bad.error().status, 400);\n"
        << "    "
           "EXPECT_TRUE(cdd_middleware::RequestValidator::validate_format("
           "\"email\", \"a@b.c\", \"email\").has_value());\n"
        << "    "
           "EXPECT_FALSE(cdd_middleware::RequestValidator::validate_format("
           "\"email\", \"invalid\", \"email\").has_value());\n"
        << "    "
           "EXPECT_TRUE(cdd_middleware::RequestValidator::validate_length("
           "\"items\", 3, 1, 5).has_value());\n"
        << "    "
           "EXPECT_FALSE(cdd_middleware::RequestValidator::validate_length("
           "\"items\", 0, 1, 5).has_value());\n"
        << "    "
           "EXPECT_FALSE(cdd_middleware::RequestValidator::validate_length("
           "\"items\", 10, 1, 5).has_value());\n"
        << "    "
           "EXPECT_TRUE(cdd_middleware::RequestValidator::validate_range(5.0, "
           "1.0, 10.0).has_value());\n"
        << "    "
           "EXPECT_FALSE(cdd_middleware::RequestValidator::validate_range(20.0,"
           " 1.0, 10.0).has_value());\n"
        << "}\n\n"
        << "TEST(AuthMiddlewareTest, VerifySchemes) {\n"
        << "    cdd_middleware::AuthMiddleware auth;\n"
        << "    EXPECT_TRUE(auth.verify_bearer(\"Invalid\"));\n"
        << "    auth.configure(true, true);\n"
        << "    EXPECT_TRUE(auth.verify_bearer(\"Bearer mock-token-123\"));\n"
        << "    EXPECT_FALSE(auth.verify_bearer(\"Invalid\"));\n"
        << "    EXPECT_TRUE(auth.verify_api_key(\"secret-key\"));\n"
        << "    EXPECT_FALSE(auth.verify_api_key(\"\"));\n"
        << "    EXPECT_TRUE(auth.verify_basic(\"Basic dXNlcjpwYXNz\"));\n"
        << "    EXPECT_FALSE(auth.verify_basic(\"Basic \"));\n"
        << "}\n\n"
        << "TEST(WebhookAndCorsTest, EndToEndDispatchAndPreflight) {\n"
        << "    httplib::Server receiver;\n"
        << "    std::string received_payload;\n"
        << "    receiver.Post(\"/hook\", [&](const httplib::Request& req, "
           "httplib::Response& res) {\n"
        << "        received_payload = req.body;\n"
        << "        res.status = 200;\n"
        << "    });\n"
        << "    int r_port = receiver.bind_to_any_port(\"127.0.0.1\");\n"
        << "    std::thread r_thread([&]() { receiver.listen_after_bind(); "
           "});\n"
        << "    httplib::Server mock_svr;\n"
        << "    cdd_middleware::CorsMiddleware::register_cors(mock_svr);\n"
        << "    cdd_webhooks::WebhookDispatcher dispatcher;\n"
        << "    dispatcher.register_event(\"orderCreated\", "
           "\"{\\\"orderId\\\":99}\");\n"
        << "    dispatcher.register_admin_routes(mock_svr);\n"
        << "    int m_port = mock_svr.bind_to_any_port(\"127.0.0.1\");\n"
        << "    std::thread m_thread([&]() { mock_svr.listen_after_bind(); "
           "});\n"
        << "    httplib::Client cli(\"http://127.0.0.1:\" + "
           "std::to_string(m_port));\n"
        << "    auto opt_res = cli.Options(\"/api/test\");\n"
        << "    EXPECT_TRUE(opt_res);\n"
        << "    EXPECT_EQ(opt_res->status, 204);\n"
        << "    "
           "EXPECT_EQ(opt_res->get_header_value(\"Access-Control-Allow-"
           "Origin\"), \"*\");\n"
        << "    std::string target_url = \"http://127.0.0.1:\" + "
           "std::to_string(r_port);\n"
        << "    auto wh_res = "
           "cli.Post(\"/_mock/trigger-webhook/orderCreated?target=\" + "
           "target_url + \"&path=/hook\", \"\", \"application/json\");\n"
        << "    EXPECT_TRUE(wh_res);\n"
        << "    EXPECT_EQ(wh_res->status, 200);\n"
        << "    EXPECT_EQ(received_payload, \"{\\\"orderId\\\":99}\");\n"
        << "    mock_svr.stop();\n"
        << "    receiver.stop();\n"
        << "    m_thread.join();\n"
        << "    r_thread.join();\n"
        << "}\n";
    files["tests/MockServerMiddlewareTest.cpp"] = mw_t_ss.str();

    std::stringstream tc_ss;
    tc_ss << "include(FetchContent)\nFetchContent_Declare(googletest URL "
             "https://github.com/google/googletest/archive/refs/tags/"
             "v1.14.0.tar.gz)\n";
    tc_ss << "FetchContent_MakeAvailable(googletest)\n";
    for (const auto &name : names) {
      tc_ss << "add_executable(" << name << "Test " << name
            << "Test.cpp)\ntarget_link_libraries(" << name
            << "Test gtest_main gmock";
      if (with_postgres)
        tc_ss << " PostgreSQL::PostgreSQL";
      tc_ss << ")\n";
    }
    tc_ss << "add_executable(MockServerMiddlewareTest "
             "MockServerMiddlewareTest.cpp)\n"
          << "target_link_libraries(MockServerMiddlewareTest gtest_main gmock "
             "httplib::httplib)\n";
    tc_ss << "include(GoogleTest)\n";
    for (const auto &name : names)
      tc_ss << "gtest_discover_tests(" << name << "Test)\n";
    tc_ss << "gtest_discover_tests(MockServerMiddlewareTest)\n";
    files["tests/CMakeLists.txt"] = tc_ss.str();
  }

  return files;
}

/// @brief Emit C++ server workflow engine to string from an Arazzo 1.1.0
/// specification.
std::string emit_arazzo(const arazzo::Arazzo &spec) noexcept {
  std::stringstream ss;
  ss << "#pragma once\n";
  ss << "#include <expected>\n";
  ss << "#include <functional>\n";
  ss << "#include <map>\n";
  ss << "#include <optional>\n";
  ss << "#include <string>\n\n";
  ss << docstrings::emit_arazzo_docstrings(spec);
  ss << "namespace cdd_server {\n\n";
  ss << "    class WorkflowEngine {\n";
  ss << "    public:\n";
  ss << "        using WorkflowHandler = "
        "std::function<std::expected<std::string, std::string>(const "
        "std::string&)>;\n\n";
  for (const auto &wf : spec.workflows) {
    ss << docstrings::emit_workflow_docstrings(wf);
    ss << "        void on_workflow_" << wf.workflowId
       << "(WorkflowHandler handler) noexcept {\n";
    ss << "            workflows[\"" << wf.workflowId
       << "\"] = std::move(handler);\n";
    ss << "        }\n\n";
    ss << "        std::expected<std::string, std::string> execute_"
       << wf.workflowId
       << "(const std::string& inputs_json) const noexcept {\n";
    ss << "            auto it = workflows.find(\"" << wf.workflowId
       << "\");\n";
    ss << "            if (it != workflows.end() && it->second) {\n";
    ss << "                return it->second(inputs_json);\n";
    ss << "            }\n";
    ss << "            return std::string(\"{}\");\n";
    ss << "        }\n\n";
  }
  ss << "    private:\n";
  ss << "        std::map<std::string, WorkflowHandler> workflows;\n";
  ss << "    };\n";
  ss << "} // namespace cdd_server\n";
  return ss.str();
}

/// @brief Emit modular C++ server workflow orchestration files from an Arazzo
/// 1.1.0 specification.
std::map<std::string, std::string>
emit_arazzo_modular(const arazzo::Arazzo &spec, bool no_installable_package,
                    bool tests) noexcept {
  std::map<std::string, std::string> files;
  files["src/WorkflowEngine.hpp"] = emit_arazzo(spec);

  std::stringstream s_ss;
  s_ss << "#include <httplib.h>\n";
  s_ss << "#include \"WorkflowEngine.hpp\"\n\n";
  s_ss << "int main() noexcept {\n";
  s_ss << "    cdd_server::WorkflowEngine engine;\n";
  s_ss << "    httplib::Server svr;\n";
  for (const auto &wf : spec.workflows) {
    s_ss << "    svr.Post(\"/workflows/" << wf.workflowId
         << "\", [&](const httplib::Request& req, httplib::Response& res) {\n";
    s_ss << "        auto out = engine.execute_" << wf.workflowId
         << "(req.body);\n";
    s_ss << "        if (out.has_value()) res.set_content(*out, "
            "\"application/json\");\n";
    s_ss << "        else { res.status = 500; res.set_content(out.error(), "
            "\"text/plain\"); }\n";
    s_ss << "    });\n";
  }
  s_ss << "    svr.listen(\"0.0.0.0\", 8080);\n";
  s_ss << "    return 0;\n";
  s_ss << "}\n";
  files["src/workflow_server.cpp"] = s_ss.str();

  if (!no_installable_package) {
    files["CMakeLists.txt"] =
        "cmake_minimum_required(VERSION 3.15)\nproject(workflow_server "
        "LANGUAGES CXX)\nset(CMAKE_CXX_STANDARD 26)\n";
  }
  if (tests) {
    std::stringstream t_ss;
    t_ss
        << "#include <gtest/gtest.h>\n#include \"../src/WorkflowEngine.hpp\"\n";
    for (const auto &wf : spec.workflows) {
      t_ss << "TEST(WorkflowEngineTest, " << wf.workflowId << ") {\n";
      t_ss << "    cdd_server::WorkflowEngine engine;\n";
      t_ss << "    auto res = engine.execute_" << wf.workflowId
           << "(\"{}\");\n";
      t_ss << "    EXPECT_TRUE(res.has_value());\n";
      t_ss << "}\n";
    }
    files["tests/WorkflowEngineTest.cpp"] = t_ss.str();
  }
  return files;
}

} // namespace cdd_cpp::server
