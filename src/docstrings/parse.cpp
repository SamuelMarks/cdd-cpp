/// @file parse.cpp
/// @brief Implementation of Doxygen docstring parsers for OpenAPI models.

#include "parse.hpp"
#include "../arazzo/parse.hpp"
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace cdd_cpp::docstrings {

/// @brief Strip leading whitespace and comment markers (`*`, `///`, `//`) from
/// a single docstring line.
/// @param line Raw docstring line.
/// @return Cleaned line content with trailing `\r`/whitespace trimmed.
std::string strip_comment_line_prefix(std::string_view line) noexcept {
  size_t pos = line.find_first_not_of(" \t");
  if (pos == std::string_view::npos) {
    return "";
  }
  std::string_view rem = line.substr(pos);
  if (rem.starts_with("///")) {
    rem.remove_prefix(3);
  } else if (rem.starts_with("//")) {
    rem.remove_prefix(2);
  } else if (rem.starts_with('*') && !rem.starts_with("*/")) {
    rem.remove_prefix(1);
  }
  if (rem.ends_with("*/")) {
    rem.remove_suffix(2);
  }
  size_t start = rem.find_first_not_of(" \t");
  if (start == std::string_view::npos) {
    return "";
  }
  size_t last = rem.find_last_not_of(" \t\r\n");
  return std::string(rem.substr(start, last - start + 1));
}

/// @brief Split a whitespace-delimited string into tokens.
/// @param sv Input string view.
/// @return Vector of non-empty whitespace-separated tokens.
static std::vector<std::string> split_ws_tokens(std::string_view sv) noexcept {
  std::vector<std::string> out;
  std::istringstream iss{std::string(sv)};
  std::string tok;
  while (iss >> tok) {
    out.push_back(tok);
  }
  return out;
}

/// @brief Extract and parse Doxygen docstring metadata for `extract_tag`.
std::optional<std::string> extract_tag(const std::string &docstring,
                                       const std::string &tag) noexcept {
  std::string needle = "@" + tag + " ";
  size_t idx = docstring.find(needle);
  if (idx == std::string::npos) {
    return std::nullopt;
  }
  size_t after = idx + needle.size();
  size_t line_end = docstring.find_first_of("\r\n", after);
  std::string raw =
      docstring.substr(after, line_end == std::string::npos ? std::string::npos
                                                            : line_end - after);
  size_t first = raw.find_first_not_of(" \t");
  if (first == std::string::npos) {
    return std::string{};
  }
  size_t last = raw.find_last_not_of(" \t\r");
  return raw.substr(first, last - first + 1);
}

/// @brief Extract and parse Doxygen docstring metadata for
/// `parse_api_docstrings`.
void parse_api_docstrings(const std::string &docstring,
                          openapi::OpenAPI &spec) noexcept {
  if (auto v = extract_tag(docstring, "title"))
    spec.info.title = *v;
  if (auto v = extract_tag(docstring, "summary"))
    spec.info.summary = *v;
  if (auto v = extract_tag(docstring, "version"))
    spec.info.version = *v;
  if (auto v = extract_tag(docstring, "description"))
    spec.info.description = *v;
  if (auto v = extract_tag(docstring, "termsOfService"))
    spec.info.termsOfService = *v;

  if (auto v = extract_tag(docstring, "contact_name")) {
    if (!spec.info.contact.has_value())
      spec.info.contact = openapi::Contact{};
    spec.info.contact->name = *v;
  }
  if (auto v = extract_tag(docstring, "contact_url")) {
    if (!spec.info.contact.has_value())
      spec.info.contact = openapi::Contact{};
    spec.info.contact->url = *v;
  }
  if (auto v = extract_tag(docstring, "contact_email")) {
    if (!spec.info.contact.has_value())
      spec.info.contact = openapi::Contact{};
    spec.info.contact->email = *v;
  }

  if (auto v = extract_tag(docstring, "license_name")) {
    if (!spec.info.license.has_value())
      spec.info.license = openapi::License{};
    spec.info.license->name = *v;
  }
  if (auto v = extract_tag(docstring, "license_identifier")) {
    if (!spec.info.license.has_value())
      spec.info.license = openapi::License{};
    spec.info.license->identifier = *v;
  }
  if (auto v = extract_tag(docstring, "license_url")) {
    if (!spec.info.license.has_value())
      spec.info.license = openapi::License{};
    spec.info.license->url = *v;
  }

  size_t servers_idx = docstring.find("@servers");
  if (servers_idx != std::string::npos) {
    spec.servers = std::vector<openapi::Server>{};
    std::string servers_block = docstring.substr(servers_idx);
    std::istringstream iss(servers_block);
    std::string line;

    openapi::Server *current_server = nullptr;

    while (std::getline(iss, line)) {
      std::string content = strip_comment_line_prefix(line);

      if (content.starts_with("@") && !content.starts_with("@servers") &&
          !content.starts_with("@server_variable")) {
        break;
      }

      if (content.starts_with("- ")) {
        std::string rest = content.substr(2);
        size_t first_ns = rest.find_first_not_of(" \t");
        rest = rest.substr(first_ns);
        openapi::Server srv;
        size_t sp = rest.find_first_of(" \t");
        if (sp == std::string::npos) {
          srv.url = rest;
        } else {
          srv.url = rest.substr(0, sp);
          size_t lparen = rest.find('(', sp);
          size_t rparen = rest.find(')', lparen);
          srv.description = rest.substr(lparen + 1, rparen - lparen - 1);
        }
        spec.servers->push_back(srv);
        current_server = &spec.servers->back();
      } else if (content.starts_with("@server_variable")) {
        std::string rest = content.substr(16);
        size_t s1 = rest.find_first_not_of(" \t");
        size_t e1 = rest.find_first_of(" \t", s1);
        std::string var_name = rest.substr(s1, e1 - s1);
        size_t s2 = rest.find_first_not_of(" \t", e1);
        size_t e2 = rest.find_first_of(" \t[", s2);
        openapi::ServerVariable var;
        if (e2 == std::string::npos) {
          var.default_value = rest.substr(s2);
        } else {
          var.default_value = rest.substr(s2, e2 - s2);
          std::string tail = rest.substr(e2);
          size_t lbrack = tail.find('[');
          size_t rbrack = tail.find(']');
          std::string desc_part = tail.substr(0, lbrack);
          size_t ds = desc_part.find_first_not_of(" \t");
          if (ds != std::string::npos) {
            size_t de = desc_part.find_last_not_of(" \t\r");
            var.description = desc_part.substr(ds, de - ds + 1);
          }
          std::string enums_str = tail.substr(lbrack + 1, rbrack - lbrack - 1);
          var.enum_values = std::vector<std::string>{};
          std::stringstream enum_ss(enums_str);
          std::string enum_val;
          while (std::getline(enum_ss, enum_val, ',')) {
            var.enum_values->push_back(enum_val);
          }
        }
        if (!current_server->variables.has_value()) {
          current_server->variables =
              std::map<std::string, openapi::ServerVariable>{};
        }
        (*current_server->variables)[var_name] = var;
      }
    }
  }

  size_t schemes_idx = docstring.find("@securitySchemes");
  if (schemes_idx != std::string::npos) {
    if (!spec.components.has_value())
      spec.components = openapi::Components{};
    if (!spec.components->securitySchemes.has_value())
      spec.components->securitySchemes =
          std::map<std::string, openapi::SecurityScheme>{};

    std::string schemes_block = docstring.substr(schemes_idx);
    std::istringstream iss(schemes_block);
    std::string line;

    while (std::getline(iss, line)) {
      std::string lcontent = strip_comment_line_prefix(line);

      if (lcontent.starts_with("@") &&
          !lcontent.starts_with("@securitySchemes") &&
          !lcontent.starts_with("@securityScheme")) {
        break;
      }

      if (lcontent.starts_with("- ")) {
        auto tokens = split_ws_tokens(lcontent.substr(2));
        openapi::SecurityScheme scheme;
        std::string key = tokens[0];
        scheme.type = tokens[1];
        if (tokens.size() >= 3)
          scheme.name = tokens[2];
        if (tokens.size() >= 4)
          scheme.in = tokens[3];
        if (tokens.size() >= 5)
          scheme.scheme = tokens[4];
        if (tokens.size() >= 6)
          scheme.bearerFormat = tokens[5];
        (*spec.components->securitySchemes)[key] = scheme;
      } else if (lcontent.starts_with("@securityScheme_description")) {
        std::string rest = lcontent.substr(27);
        size_t s1 = rest.find_first_not_of(" \t");
        size_t e1 = rest.find_first_of(" \t", s1);
        std::string key = rest.substr(s1, e1 - s1);
        size_t s2 = rest.find_first_not_of(" \t", e1);
        if (spec.components->securitySchemes->contains(key)) {
          (*spec.components->securitySchemes)[key].description =
              rest.substr(s2);
        }
      } else if (lcontent.starts_with("@securityScheme_openIdConnectUrl")) {
        auto tokens = split_ws_tokens(lcontent.substr(32));
        if (spec.components->securitySchemes->contains(tokens[0])) {
          (*spec.components->securitySchemes)[tokens[0]].openIdConnectUrl =
              tokens[1];
        }
      } else if (lcontent.starts_with("@securityScheme_oauth2MetadataUrl")) {
        auto tokens = split_ws_tokens(lcontent.substr(33));
        if (spec.components->securitySchemes->contains(tokens[0])) {
          (*spec.components->securitySchemes)[tokens[0]].oauth2MetadataUrl =
              tokens[1];
        }
      } else if (lcontent.starts_with("@securityScheme_deprecated")) {
        auto tokens = split_ws_tokens(lcontent.substr(26));
        if (spec.components->securitySchemes->contains(tokens[0])) {
          (*spec.components->securitySchemes)[tokens[0]].deprecated = true;
        }
      }
    }
  }
}

/// @brief Extract and parse Doxygen docstring metadata for
/// `parse_operation_docstrings`.
void parse_operation_docstrings(const std::string &docstring,
                                openapi::Operation &op) noexcept {
  if (auto v = extract_tag(docstring, "summary"))
    op.summary = *v;
  if (auto v = extract_tag(docstring, "description"))
    op.description = *v;

  if (auto tags_str = extract_tag(docstring, "tags")) {
    op.tags = std::vector<std::string>{};
    std::stringstream ts(*tags_str);
    std::string tag;
    while (std::getline(ts, tag, ',')) {
      op.tags->push_back(tag);
    }
  }

  auto ensure_resp = [&](const std::string &code) -> openapi::Response & {
    if (!op.responses.has_value())
      op.responses = std::map<std::string, openapi::Response>{};
    return (*op.responses)[code];
  };

  std::istringstream iss(docstring);
  std::string line;
  while (std::getline(iss, line)) {
    std::string content = strip_comment_line_prefix(line);
    if (content == "@deprecated") {
      op.deprecated = true;
    } else if (content.starts_with("@security ")) {
      std::string rest = content.substr(10);
      size_t s1 = rest.find_first_not_of(" \t");
      size_t e1 = rest.find_first_of(" \t", s1);
      std::string scheme = rest.substr(s1, e1 - s1);
      size_t lb = rest.find('[', e1);
      size_t rb = rest.find(']', lb);
      std::string scopes_str = rest.substr(lb + 1, rb - lb - 1);
      std::vector<std::string> scopes;
      std::stringstream ss(scopes_str);
      std::string scope;
      while (std::getline(ss, scope, ',')) {
        scopes.push_back(scope);
      }
      if (!op.security.has_value())
        op.security = std::vector<openapi::SecurityRequirement>{};
      openapi::SecurityRequirement req;
      req[scheme] = scopes;
      op.security->push_back(req);
    } else if (content.starts_with("@response_content ")) {
      auto tokens = split_ws_tokens(content.substr(18));
      auto &resp = ensure_resp(tokens[0]);
      if (!resp.content.has_value()) {
        resp.content = std::map<std::string, openapi::MediaType>{};
      }
      (*resp.content)[tokens[1]] = openapi::MediaType{};
    } else if (content.starts_with("@response ") ||
               content.starts_with("@return ")) {
      size_t dash = content.find(" - ");
      if (dash != std::string::npos) {
        size_t sp = content.find_first_of(" \t");
        size_t s1 = content.find_first_not_of(" \t", sp);
        size_t e1 = content.find_first_of(" \t", s1);
        std::string code = content.substr(s1, e1 - s1);
        size_t ds = content.find_first_not_of(" \t", dash + 3);
        ensure_resp(code).description = content.substr(ds);
      }
    }
  }
}

/// @brief Extract and parse Doxygen docstring metadata for
/// `parse_path_docstrings`.
void parse_path_docstrings(const std::string &docstring,
                           openapi::PathItem &pi) noexcept {
  if (auto v = extract_tag(docstring, "summary"))
    pi.summary = *v;
  if (auto v = extract_tag(docstring, "description"))
    pi.description = *v;
}

/// @brief Parse schema/field-level metadata into an OpenAPI Schema object.
void parse_schema_docstrings(const std::string &docstring,
                             openapi::Schema &schema) noexcept {
  if (auto v = extract_tag(docstring, "description")) {
    schema.description = *v;
  } else if (!docstring.empty() && docstring.find('@') == std::string::npos) {
    schema.description = docstring;
  }
  if (auto v = extract_tag(docstring, "example"))
    schema.example = *v;
  if (auto v = extract_tag(docstring, "format"))
    schema.format = *v;

  auto ensure_xml = [&]() -> openapi::XML & {
    if (!schema.xml.has_value())
      schema.xml = openapi::XML{};
    return *schema.xml;
  };
  if (auto v = extract_tag(docstring, "xml_nodeType"))
    ensure_xml().nodeType = *v;
  if (auto v = extract_tag(docstring, "xml.nodeType"))
    ensure_xml().nodeType = *v;
  if (auto v = extract_tag(docstring, "xml_name"))
    ensure_xml().name = *v;
  if (auto v = extract_tag(docstring, "xml.name"))
    ensure_xml().name = *v;
  if (auto v = extract_tag(docstring, "xml_namespace"))
    ensure_xml().namespace_url = *v;
  if (auto v = extract_tag(docstring, "xml.namespace_url"))
    ensure_xml().namespace_url = *v;
  if (auto v = extract_tag(docstring, "xml_prefix"))
    ensure_xml().prefix = *v;
  if (auto v = extract_tag(docstring, "xml.prefix"))
    ensure_xml().prefix = *v;
  if (docstring.find("@xml_attribute true") != std::string::npos ||
      docstring.find("@xml.attribute true") != std::string::npos)
    ensure_xml().attribute = true;
  if (docstring.find("@xml_wrapped true") != std::string::npos ||
      docstring.find("@xml.wrapped true") != std::string::npos)
    ensure_xml().wrapped = true;

  auto ensure_ext_docs = [&]() -> openapi::ExternalDocumentation & {
    if (!schema.externalDocs.has_value())
      schema.externalDocs = openapi::ExternalDocumentation{};
    return *schema.externalDocs;
  };
  if (auto v = extract_tag(docstring, "externalDocs_url"))
    ensure_ext_docs().url = *v;
  if (auto v = extract_tag(docstring, "externalDocs.url"))
    ensure_ext_docs().url = *v;
  if (auto v = extract_tag(docstring, "externalDocs_description"))
    ensure_ext_docs().description = *v;
  if (auto v = extract_tag(docstring, "externalDocs.description"))
    ensure_ext_docs().description = *v;

  auto ensure_disc = [&]() -> openapi::Discriminator & {
    if (!schema.discriminator.has_value())
      schema.discriminator = openapi::Discriminator{};
    return *schema.discriminator;
  };
  if (auto v = extract_tag(docstring, "discriminator_propertyName"))
    ensure_disc().propertyName = *v;
  if (auto v = extract_tag(docstring, "discriminator.propertyName"))
    ensure_disc().propertyName = *v;

  std::istringstream iss(docstring);
  std::string line;
  while (std::getline(iss, line)) {
    std::string content = strip_comment_line_prefix(line);
    if (content.starts_with("@discriminator_mapping ")) {
      auto tokens = split_ws_tokens(content.substr(23));
      if (!ensure_disc().mapping.has_value()) {
        ensure_disc().mapping = std::map<std::string, std::string>{};
      }
      schema.discriminator->mapping->insert({tokens[0], tokens[1]});
    } else if (content.starts_with("@discriminator.mapping ")) {
      auto tokens = split_ws_tokens(content.substr(23));
      size_t colon = tokens[0].find(':');
      if (!ensure_disc().mapping.has_value()) {
        ensure_disc().mapping = std::map<std::string, std::string>{};
      }
      schema.discriminator->mapping->insert(
          {tokens[0].substr(0, colon), tokens[0].substr(colon + 1)});
    }
  }
}

/// @brief Extract and parse Doxygen docstring metadata for
/// `parse_arazzo_docstrings`.
void parse_arazzo_docstrings(const std::string &docstring,
                             arazzo::Arazzo &spec) noexcept {
  auto full_json = extract_tag(docstring, "arazzo_spec_json");
  if (full_json.has_value()) {
    auto parsed = arazzo::parse(*full_json);
    if (parsed.has_value()) {
      spec = std::move(*parsed);
      return;
    }
  }
  auto v_arazzo = extract_tag(docstring, "arazzo");
  if (v_arazzo.has_value())
    spec.arazzo = *v_arazzo;
  auto v_title = extract_tag(docstring, "arazzo_title");
  if (v_title.has_value())
    spec.info.title = *v_title;
  auto v_ver = extract_tag(docstring, "arazzo_version");
  if (v_ver.has_value())
    spec.info.version = *v_ver;
}

/// @brief Extract and parse Doxygen docstring metadata for
/// `parse_workflow_docstrings`.
void parse_workflow_docstrings(const std::string &docstring,
                               arazzo::Workflow &wf) noexcept {
  auto v_id = extract_tag(docstring, "workflowId");
  if (v_id.has_value())
    wf.workflowId = *v_id;
  auto v_sum = extract_tag(docstring, "summary");
  if (v_sum.has_value())
    wf.summary = *v_sum;
  auto v_desc = extract_tag(docstring, "description");
  if (v_desc.has_value())
    wf.description = *v_desc;
}

/// @brief Extract and parse Doxygen docstring metadata for `parse`.
openapi::OpenAPI parse(const std::string &input) noexcept {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  parse_api_docstrings(input, spec);
  return spec;
}

} // namespace cdd_cpp::docstrings
