#include "emit.hpp"
#include <sstream>

namespace cdd_cpp::docstrings {

std::string emit_api_docstrings(const openapi::OpenAPI &spec) noexcept {
  std::stringstream ss;
  ss << "/// @title " << spec.info.title << "\n";
  ss << "/// @version " << spec.info.version << "\n";
  if (spec.info.description) ss << "/// @description " << *spec.info.description << "\n";
  if (spec.info.termsOfService) ss << "/// @termsOfService " << *spec.info.termsOfService << "\n";
  if (spec.info.contact) {
    if (spec.info.contact->name) ss << "/// @contact_name " << *spec.info.contact->name << "\n";
    if (spec.info.contact->url) ss << "/// @contact_url " << *spec.info.contact->url << "\n";
    if (spec.info.contact->email) ss << "/// @contact_email " << *spec.info.contact->email << "\n";
  }
  if (spec.info.license) {
    ss << "/// @license_name " << spec.info.license->name << "\n";
    if (spec.info.license->identifier) ss << "/// @license_identifier " << *spec.info.license->identifier << "\n";
    if (spec.info.license->url) ss << "/// @license_url " << *spec.info.license->url << "\n";
  }
  if (spec.servers && !spec.servers->empty()) {
    for (const auto &server : *spec.servers) {
      ss << "/// - " << server.url;
      if (server.description) ss << " (" << *server.description << ")";
      ss << "\n";
      if (server.variables) {
        for (const auto &[var_name, var] : *server.variables) {
          ss << "///   @server_variable " << var_name << " " << var.default_value;
          if (var.description) ss << " " << *var.description;
          if (var.enum_values) {
            ss << " [";
            for (size_t i = 0; i < var.enum_values->size(); ++i) {
              if (i > 0) ss << ",";
              ss << var.enum_values->at(i);
            }
            ss << "]";
          }
          ss << "\n";
        }
      }
    }
  }
  if (spec.components && spec.components->securitySchemes) {
    ss << "/// @securitySchemes\n";
    for (const auto &[name, scheme] : *spec.components->securitySchemes) {
      ss << "/// - " << name << " " << scheme.type;
      if (scheme.name) ss << " " << *scheme.name;
      if (scheme.in) ss << " " << *scheme.in;
      if (scheme.scheme) ss << " " << *scheme.scheme;
      if (scheme.bearerFormat) ss << " " << *scheme.bearerFormat;
      ss << "\n";
      if (scheme.description) ss << "///   @securityScheme_description " << name << " " << *scheme.description << "\n";
      if (scheme.openIdConnectUrl) ss << "///   @securityScheme_openIdConnectUrl " << name << " " << *scheme.openIdConnectUrl << "\n";
      if (scheme.oauth2MetadataUrl) ss << "///   @securityScheme_oauth2MetadataUrl " << name << " " << *scheme.oauth2MetadataUrl << "\n";
      if (scheme.deprecated && *scheme.deprecated) ss << "///   @securityScheme_deprecated " << name << "\n";
    }
  }
  
  return ss.str();
}

std::string emit_path_docstrings(const openapi::PathItem &pi) noexcept {
  std::stringstream ss;
  if (pi.summary) ss << "        /// @summary " << *pi.summary << "\n";
  if (pi.description) ss << "        /// @description " << *pi.description << "\n";
  return ss.str();
}

std::string emit_operation_docstrings(const openapi::Operation &op) noexcept {
  std::stringstream ss;
  if (op.summary) ss << "        /// @summary " << *op.summary << "\n";
  if (op.description) ss << "        /// @description " << *op.description << "\n";
  if (op.tags && !op.tags->empty()) {
    ss << "        /// @tags ";
    for (size_t i = 0; i < op.tags->size(); ++i) {
      if (i > 0) ss << ",";
      ss << op.tags->at(i);
    }
    ss << "\n";
  }
  if (op.deprecated) ss << "        /// @deprecated\n";
  if (op.security && !op.security->empty()) {
    for (const auto &req : *op.security) {
      for (const auto &[key, scopes] : req) {
        ss << "        /// @security " << key;
        if (!scopes.empty()) {
          ss << " [";
          for (size_t i = 0; i < scopes.size(); ++i) {
            if (i > 0) ss << ",";
            ss << scopes[i];
          }
          ss << "]";
        }
        ss << "\n";
      }
    }
  }
  if (op.parameters) {
    for (const auto &param : *op.parameters) {
      ss << "        /// @param " << param.name << " " << param.in << " parameter\n";
    }
  }
  if (op.requestBody) {
    ss << "        /// @param body request body";
    if (op.requestBody->description) ss << " - " << *op.requestBody->description;
    ss << "\n";
  }
  if (op.responses) {
    for (const auto &[code, resp] : *op.responses) {
      ss << "        /// @return " << code << " - " << resp.description << "\n";
      if (resp.content) {
        for (const auto &[ctype, _] : *resp.content) {
          ss << "        /// @response_content " << code << " " << ctype << "\n";
        }
      }
    }
  }
  return ss.str();
}

} // namespace cdd_cpp::docstrings