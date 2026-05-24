// GCOV_EXCL_BR_START

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
  assert(spec.components->schemas->find("Test1") !=
         spec.components->schemas->end());
  assert(spec.components->schemas->find("TestExtUrl") !=
         spec.components->schemas->end());

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

  openapi::Schema circ1;
  circ1.type = "object";
  circ1.properties = std::make_shared<std::map<std::string, openapi::Schema>>();
  openapi::Schema prop_circ2;
  prop_circ2.ref = openapi::Reference{"#/components/schemas/Circ2",
                                      std::nullopt, std::nullopt};
  circ1.properties->insert({"c2", prop_circ2});

  openapi::Schema circ2;
  circ2.type = "object";
  circ2.properties = std::make_shared<std::map<std::string, openapi::Schema>>();
  openapi::Schema prop_circ1;
  prop_circ1.ref = openapi::Reference{"#/components/schemas/Circ1",
                                      std::nullopt, std::nullopt};
  circ2.properties->insert({"c1", prop_circ1});

  spec.components->schemas->insert({"Circ1", circ1});
  spec.components->schemas->insert({"Circ2", circ2});

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

  {
    cdd_cpp::openapi::Reference ref1;
    ref1.ref = "test";
    cdd_cpp::openapi::Reference ref2 = ref1;
    ref1 = ref2;
    ref1 = std::move(ref2);
    assert(ref1.ref == "test");
    cdd_cpp::openapi::Contact c1;
    c1.name = "test";
    cdd_cpp::openapi::Contact c2 = c1;
    c1 = c2;
    c1 = std::move(c2);
    assert(c1.name == "test");
    cdd_cpp::openapi::License l1;
    l1.name = "test";
    cdd_cpp::openapi::License l2 = l1;
    l1 = l2;
    l1 = std::move(l2);
    assert(l1.name == "test");
    cdd_cpp::openapi::Info inf1;
    inf1.title = "test";
    cdd_cpp::openapi::Info inf2 = inf1;
    inf1 = inf2;
    inf1 = std::move(inf2);
    assert(inf1.title == "test");
    cdd_cpp::openapi::ServerVariable sv1;
    sv1.default_value = "test";
    cdd_cpp::openapi::ServerVariable sv2 = sv1;
    sv1 = sv2;
    sv1 = std::move(sv2);
    assert(sv1.default_value == "test");
    cdd_cpp::openapi::Server s1;
    s1.url = "test";
    cdd_cpp::openapi::Server s2 = s1;
    s1 = s2;
    s1 = std::move(s2);
    assert(s1.url == "test");
    cdd_cpp::openapi::ExternalDocumentation e1;
    e1.url = "test";
    cdd_cpp::openapi::ExternalDocumentation e2 = e1;
    e1 = e2;
    e1 = std::move(e2);
    assert(e1.url == "test");
    cdd_cpp::openapi::Tag t1;
    t1.name = "test";
    cdd_cpp::openapi::Tag t2 = t1;
    t1 = t2;
    t1 = std::move(t2);
    assert(t1.name == "test");
    cdd_cpp::openapi::Discriminator disc1;
    disc1.propertyName = "test";
    cdd_cpp::openapi::Discriminator disc2 = disc1;
    disc1 = disc2;
    disc1 = std::move(disc2);
    assert(disc1.propertyName == "test");
    cdd_cpp::openapi::XML x1;
    x1.name = "test";
    cdd_cpp::openapi::XML x2 = x1;
    x1 = x2;
    x1 = std::move(x2);
    assert(x1.name == "test");
    cdd_cpp::openapi::Schema sch1;
    sch1.type = "test";
    cdd_cpp::openapi::Schema sch2 = sch1;
    sch1 = sch2;
    sch1 = std::move(sch2);
    assert(sch1.type == "test");
    cdd_cpp::openapi::Example ex1;
    ex1.summary = "test";
    cdd_cpp::openapi::Example ex2 = ex1;
    ex1 = ex2;
    ex1 = std::move(ex2);
    assert(ex1.summary == "test");
    cdd_cpp::openapi::Encoding enc1;
    enc1.contentType = "test";
    cdd_cpp::openapi::Encoding enc2 = enc1;
    enc1 = enc2;
    enc1 = std::move(enc2);
    assert(enc1.contentType == "test");
    cdd_cpp::openapi::MediaType mt1;
    cdd_cpp::openapi::MediaType mt2 = mt1;
    mt1 = mt2;
    mt1 = std::move(mt2);
    cdd_cpp::openapi::Header hdr1;
    hdr1.description = "test";
    cdd_cpp::openapi::Header hdr2 = hdr1;
    hdr1 = hdr2;
    hdr1 = std::move(hdr2);
    assert(hdr1.description == "test");
    cdd_cpp::openapi::RequestBody r1;
    r1.description = "test";
    cdd_cpp::openapi::RequestBody r2 = r1;
    r1 = r2;
    r1 = std::move(r2);
    assert(r1.description == "test");
    cdd_cpp::openapi::Link lk1;
    lk1.operationId = "test";
    cdd_cpp::openapi::Link lk2 = lk1;
    lk1 = lk2;
    lk1 = std::move(lk2);
    assert(lk1.operationId == "test");
    cdd_cpp::openapi::Response rsq1;
    rsq1.description = "test";
    cdd_cpp::openapi::Response rsq2 = rsq1;
    rsq1 = rsq2;
    rsq1 = std::move(rsq2);
    assert(rsq1.description == "test");
    cdd_cpp::openapi::Parameter param1;
    param1.name = "test";
    cdd_cpp::openapi::Parameter param2 = param1;
    param1 = param2;
    param1 = std::move(param2);
    assert(param1.name == "test");
    cdd_cpp::openapi::OAuthFlow oaf1;
    oaf1.authorizationUrl = "test";
    cdd_cpp::openapi::OAuthFlow oaf2(oaf1);
    cdd_cpp::openapi::OAuthFlow oaf3(std::move(oaf1));
    oaf1 = oaf2;
    oaf1 = std::move(oaf2);
    assert(oaf1.authorizationUrl == "test");
    cdd_cpp::openapi::OAuthFlows oafs1;
    oafs1.implicit = oaf3;
    cdd_cpp::openapi::OAuthFlows oafs2(oafs1);
    cdd_cpp::openapi::OAuthFlows oafs3(std::move(oafs1));
    oafs1 = oafs2;
    oafs1 = std::move(oafs2);
    cdd_cpp::openapi::SecurityScheme ssc1;
    ssc1.type = "test";
    cdd_cpp::openapi::SecurityScheme ssc2 = ssc1;
    ssc1 = ssc2;
    ssc1 = std::move(ssc2);
    assert(ssc1.type == "test");
    cdd_cpp::openapi::Operation op1;
    op1.operationId = "test";
    cdd_cpp::openapi::Operation op2 = op1;
    op1 = op2;
    op1 = std::move(op2);
    assert(op1.operationId == "test");
    cdd_cpp::openapi::PathItem pi1;
    pi1.summary = "test";
    cdd_cpp::openapi::PathItem pi2 = pi1;
    pi1 = pi2;
    pi1 = std::move(pi2);
    assert(pi1.summary == "test");
    cdd_cpp::openapi::Components comp1;
    cdd_cpp::openapi::Components comp2 = comp1;
    comp1 = comp2;
    comp1 = std::move(comp2);
    cdd_cpp::openapi::OpenAPI oas1;
    oas1.openapi = "test";
    cdd_cpp::openapi::OpenAPI oas2 = oas1;
    oas1 = oas2;
    oas1 = std::move(oas2);
    assert(oas1.openapi == "test");
  }

  std::cout << "models::test_emit passed.\n";
}
} // namespace cdd_cpp::models
// GCOV_EXCL_BR_STOP
