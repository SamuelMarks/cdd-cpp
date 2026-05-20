#include "../models/emit.hpp"
#include "../models/parse.hpp"
#include <cassert>
#include <iostream>

namespace cdd_cpp::models {
void test_parse() {
  openapi::OpenAPI spec;
  utils::CppAST ast;

  utils::CppClass cls1;
  cls1.name = "Test1";
  cls1.docstring = "/// @description Test description\n"
                   "/// @example my_example\n"
                   "/// @externalDocs.description More info\n"
                   "/// @externalDocs.url https://docs\n"
                   "/// @xml.name name\n"
                   "/// @xml.namespace_url https://xml\n"
                   "/// @xml.prefix prefix\n"
                   "/// @xml.attribute true\n"
                   "/// @xml.wrapped true\n"
                   "/// @discriminator.propertyName prop\n"
                   "/// @discriminator.mapping k1:v1\n";
  
  cls1.fields.push_back({"int", "f_int", "/// @description field int"});
  cls1.fields.push_back({"double", "f_double", ""});
  cls1.fields.push_back({"bool", "f_bool", ""});
  cls1.fields.push_back({"std::string", "f_str", ""});
  cls1.fields.push_back({"std::optional<int>", "f_opt", ""});
  cls1.fields.push_back({"std::vector<int>", "f_vec_int", ""});
  cls1.fields.push_back({"std::vector<double>", "f_vec_double", ""});
  cls1.fields.push_back({"std::vector<bool>", "f_vec_bool", ""});
  cls1.fields.push_back({"std::vector<std::string>", "f_vec_str", ""});
  cls1.fields.push_back({"std::vector<Cust>", "f_vec_cust", ""});
  cls1.fields.push_back({"Cust", "f_cust", ""});
  
  utils::CppClass cls_ext_url;
  cls_ext_url.name = "TestExtUrl";
  cls_ext_url.docstring = "/// @externalDocs.url url\n";

  utils::CppClass cls_xml_ns;
  cls_xml_ns.name = "TestXmlNs";
  cls_xml_ns.docstring = "/// @xml.namespace_url url\n";
  
  utils::CppClass cls_xml_pref;
  cls_xml_pref.name = "TestXmlPref";
  cls_xml_pref.docstring = "/// @xml.prefix pref\n";

  utils::CppClass cls_xml_attr;
  cls_xml_attr.name = "TestXmlAttr";
  cls_xml_attr.docstring = "/// @xml.attribute true\n";

  utils::CppClass cls_xml_wrap;
  cls_xml_wrap.name = "TestXmlWrap";
  cls_xml_wrap.docstring = "/// @xml.wrapped true\n";

  utils::CppClass cls_disc_map;
  cls_disc_map.name = "TestDiscMap";
  cls_disc_map.docstring = "/// @discriminator.mapping k1:v1\n";

  utils::CppClass cls_none;
  cls_none.name = "TestNone";
                   
  ast.classes.push_back(cls1);
  ast.classes.push_back(cls_ext_url);
  ast.classes.push_back(cls_xml_ns);
  ast.classes.push_back(cls_xml_pref);
  ast.classes.push_back(cls_xml_attr);
  ast.classes.push_back(cls_xml_wrap);
  ast.classes.push_back(cls_disc_map);
  ast.classes.push_back(cls_none);

  utils::parse_schemas(ast, spec);
  
  assert(spec.components.has_value());
  assert(spec.components->schemas.has_value());
  assert(spec.components->schemas->find("Test1") != spec.components->schemas->end());
  assert(spec.components->schemas->find("TestExtUrl") != spec.components->schemas->end());

  std::cout << "models::test_parse passed.\n";
}

void test_emit() {
  openapi::OpenAPI spec;
  spec.openapi = "3.2.0";
  spec.info.title = "Test";
  spec.info.version = "1.0";
  spec.components = openapi::Components{};
  spec.components->schemas = std::map<std::string, openapi::Schema>{};

  openapi::Schema s;
  s.type = "object";
  s.description = "Test description\r\nline2";
  s.example = "{\"name\": \"test\"}";
  s.externalDocs = openapi::ExternalDocumentation{"More info", "https://docs"};
  s.xml = openapi::XML{"name", "https://xml", "prefix", true, false};
  s.discriminator = openapi::Discriminator{
      "prop", std::map<std::string, std::string>{{"k1", "v1"}}};
  s.properties = std::make_shared<std::map<std::string, openapi::Schema>>();
  s.required = std::vector<std::string>{"name",  "age", "active",
                                        "score", "ref", "array"};

  openapi::Schema p;
  p.type = "string";
  p.description = "Name field";
  s.properties->insert({"name", p});

  openapi::Schema p2;
  p2.type = "integer";
  s.properties->insert({"age", p2});

  openapi::Schema p3;
  p3.type = "boolean";
  s.properties->insert({"active", p3});

  openapi::Schema p4;
  p4.type = "number";
  s.properties->insert({"score", p4});

  openapi::Schema p_ref;
  p_ref.ref = openapi::Reference{"#/components/schemas/OtherStruct",
                                 std::nullopt, std::nullopt};
  s.properties->insert({"ref", p_ref});

  openapi::Schema p_ref2;
  p_ref2.ref = openapi::Reference{"no_slash", std::nullopt, std::nullopt};
  s.properties->insert({"ref2", p_ref2});

  openapi::Schema p_arr;
  p_arr.type = "array";
  p_arr.items = std::make_shared<openapi::Schema>(p2);
  s.properties->insert({"array", p_arr});

  openapi::Schema p_arr2;
  p_arr2.type = "array";
  s.properties->insert({"array2", p_arr2});

  openapi::Schema p_none;
  s.properties->insert({"none", p_none});

  openapi::Schema p_opt;
  p_opt.type = "string";
  s.properties->insert({"opt_string", p_opt});

  openapi::Schema p_rand;
  p_rand.type = "random";
  s.properties->insert({"rand", p_rand});

  spec.components->schemas->insert({"TestStruct", s});

  std::string code = emit(spec);
  assert(code.find("struct TestStruct") != std::string::npos);
  assert(code.find("std::string name;") != std::string::npos);
  assert(code.find("int age;") != std::string::npos);
  assert(code.find("bool active;") != std::string::npos);
  assert(code.find("double score;") != std::string::npos);
  assert(code.find("OtherStruct ref;") != std::string::npos);
  assert(code.find("std::vector<int> array;") != std::string::npos);
  assert(code.find("std::optional<std::string> opt_string;") !=
         std::string::npos);
  assert(code.find("OtherStruct ref;") != std::string::npos);
  assert(code.find("std::vector<int> array;") != std::string::npos);
  assert(code.find("std::optional<std::string> opt_string;") !=
         std::string::npos);
  assert(code.find("std::optional<no_slash> ref2;") != std::string::npos);
  assert(code.find("std::optional<std::vector<std::string>> array2;") !=
         std::string::npos);
  assert(code.find("std::optional<std::string> none;") != std::string::npos);
  assert(code.find("std::optional<std::string> rand;") != std::string::npos);
  assert(code.find("std::optional<no_slash> ref2;") != std::string::npos);
  assert(code.find("std::optional<std::vector<std::string>> array2;") !=
         std::string::npos);
  assert(code.find("std::optional<std::string> none;") != std::string::npos);

  assert(code.find("/// @description Test description") != std::string::npos);
  assert(code.find("/// @example {\"name\": \"test\"}") != std::string::npos);
  assert(code.find("/// @externalDocs.description More info") !=
         std::string::npos);
  assert(code.find("/// @externalDocs.url https://docs") != std::string::npos);
  assert(code.find("/// @xml.name name") != std::string::npos);
  assert(code.find("/// @xml.namespace_url https://xml") != std::string::npos);
  assert(code.find("/// @xml.prefix prefix") != std::string::npos);
  assert(code.find("/// @xml.attribute true") != std::string::npos);
  assert(code.find("/// @xml.wrapped false") != std::string::npos);
  assert(code.find("/// @discriminator.propertyName prop") !=
         std::string::npos);
  assert(code.find("/// @discriminator.mapping k1:v1") != std::string::npos);
  assert(code.find("/// @description Name field") != std::string::npos);

  std::cout << "models::test_emit passed.\n";
}
} // namespace cdd_cpp::models