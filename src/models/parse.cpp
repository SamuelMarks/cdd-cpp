#include "../utils/cpp_ast.hpp"
#include "../openapi/models.hpp"
#include <regex>
#include <string>

namespace cdd_cpp::utils {
    
    std::optional<std::string> extract(const std::string& docstring, const std::string& tag) {
        std::regex re(R"(@)" + tag + R"(\s+([^\n\r]+))");
        std::smatch match;
        if (std::regex_search(docstring, match, re)) {
            std::string res = match[1].str();
            res.erase(res.find_last_not_of(" \t\r") + 1);
            return res;
        }
        return std::nullopt;
    }

    void parse_schemas(const CppAST& ast, openapi::OpenAPI& spec) noexcept {
        for (const auto& cls : ast.classes) {
            if (cls.name == "Client" || cls.name == "Router") continue; // skip main classes
            
            if (!spec.components.has_value()) spec.components = openapi::Components{};
            if (!spec.components->schemas.has_value()) spec.components->schemas = std::map<std::string, openapi::Schema>{};
            
            openapi::Schema schema;
            schema.type = "object";
            schema.properties = std::make_shared<std::map<std::string, openapi::Schema>>();
            std::vector<std::string> required;
            
            if (auto v = extract(cls.docstring, "description")) schema.description = *v;
            if (auto v = extract(cls.docstring, "example")) schema.example = *v;
            
            if (auto vd = extract(cls.docstring, "externalDocs.description")) {
                if (!schema.externalDocs.has_value()) schema.externalDocs = openapi::ExternalDocumentation{};
                schema.externalDocs->description = *vd;
            }
            if (auto vu = extract(cls.docstring, "externalDocs.url")) {
                if (!schema.externalDocs.has_value()) schema.externalDocs = openapi::ExternalDocumentation{};
                schema.externalDocs->url = *vu;
            }
            
            if (auto vn = extract(cls.docstring, "xml.name")) {
                if (!schema.xml.has_value()) schema.xml = openapi::XML{};
                schema.xml->name = *vn;
            }
            if (auto vns = extract(cls.docstring, "xml.namespace_url")) {
                if (!schema.xml.has_value()) schema.xml = openapi::XML{};
                schema.xml->namespace_url = *vns;
            }
            if (auto vp = extract(cls.docstring, "xml.prefix")) {
                if (!schema.xml.has_value()) schema.xml = openapi::XML{};
                schema.xml->prefix = *vp;
            }
            if (auto va = extract(cls.docstring, "xml.attribute")) {
                if (!schema.xml.has_value()) schema.xml = openapi::XML{};
                schema.xml->attribute = (*va == "true");
            }
            if (auto vw = extract(cls.docstring, "xml.wrapped")) {
                if (!schema.xml.has_value()) schema.xml = openapi::XML{};
                schema.xml->wrapped = (*vw == "true");
            }
            
            if (auto vdp = extract(cls.docstring, "discriminator.propertyName")) {
                if (!schema.discriminator.has_value()) schema.discriminator = openapi::Discriminator{};
                schema.discriminator->propertyName = *vdp;
            }
            
            std::regex disc_map_re(R"(@discriminator\.mapping\s+([^\s:]+):([^\s]+))");
            auto disc_begin = std::sregex_iterator(cls.docstring.begin(), cls.docstring.end(), disc_map_re);
            auto disc_end = std::sregex_iterator();
            if (disc_begin != disc_end) {
                if (!schema.discriminator.has_value()) schema.discriminator = openapi::Discriminator{};
                if (!schema.discriminator->mapping.has_value()) schema.discriminator->mapping = std::map<std::string, std::string>{};
                for (std::sregex_iterator it = disc_begin; it != disc_end; ++it) {
                    schema.discriminator->mapping->insert({(*it)[1].str(), (*it)[2].str()});
                }
            }

            for (const auto& field : cls.fields) {
                openapi::Schema prop;
                std::string ftype = field.type;
                bool is_optional = false;
                if (ftype.find("std::optional<") == 0 && ftype.find(">") == ftype.length() - 1) {
                    is_optional = true;
                    ftype = ftype.substr(14, ftype.length() - 15);
                }
                
                if (!is_optional) required.push_back(field.name);
                
                if (ftype == "int") prop.type = "integer";
                else if (ftype == "double") prop.type = "number";
                else if (ftype == "bool") prop.type = "boolean";
                else if (ftype == "std::string") prop.type = "string";
                else if (ftype.find("std::vector<") == 0 && ftype.find(">") == ftype.length() - 1) {
                    prop.type = "array";
                    std::string item_type = ftype.substr(12, ftype.length() - 13);
                    openapi::Schema item_schema;
                    if (item_type == "int") item_schema.type = "integer";
                    else if (item_type == "double") item_schema.type = "number";
                    else if (item_type == "bool") item_schema.type = "boolean";
                    else if (item_type == "std::string") item_schema.type = "string";
                    else {
                        item_schema.ref = openapi::Reference{"#/components/schemas/" + item_type, std::nullopt, std::nullopt};
                    }
                    prop.items = std::make_shared<openapi::Schema>(item_schema);
                } else {
                    prop.ref = openapi::Reference{"#/components/schemas/" + ftype, std::nullopt, std::nullopt};
                }
                
                if (auto v = extract(field.docstring, "description")) prop.description = *v;
                
                schema.properties->insert({field.name, prop});
            }
            
            if (!required.empty()) {
                schema.required = required;
            }
            
            spec.components->schemas->insert({cls.name, schema});
        }
    }
}
