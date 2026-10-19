/// @file arazzo_test.cpp
/// @brief Exhaustive unit tests for Arazzo 1.1.0 models, runtime expressions,
/// parser, emitter, docstrings, SDK, CLI, Server, Functions, Mocks, and CLI
/// commands.

#include "../arazzo/emit.hpp"
#include "../arazzo/expressions.hpp"
#include "../arazzo/models.hpp"
#include "../arazzo/parse.hpp"
#include "../cli/api.hpp"
#include "../client_sdk/emit.hpp"
#include "../client_sdk/parse.hpp"
#include "../client_sdk_cli/emit.hpp"
#include "../client_sdk_cli/parse.hpp"
#include "../docstrings/emit.hpp"
#include "../docstrings/parse.hpp"
#include "../functions/emit.hpp"
#include "../functions/parse.hpp"
#include "../mocks/emit.hpp"
#include "../mocks/parse.hpp"
#include "../server/emit.hpp"
#include "../server/parse.hpp"
#include "../utils/cpp_parser.hpp"
#include "../utils/error.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace cdd_cpp::arazzo {

static void test_arazzo_enums_and_errors() noexcept {
  assert(utils::error_code_to_string(utils::ErrorCode::InvalidArazzoDocument) ==
         "InvalidArazzoDocument");
  assert(
      utils::error_code_to_string(utils::ErrorCode::InvalidRuntimeExpression) ==
      "InvalidRuntimeExpression");
  assert(utils::error_code_to_string(
             utils::ErrorCode::UnresolvedSourceDescription) ==
         "UnresolvedSourceDescription");
  assert(utils::error_code_to_string(
             utils::ErrorCode::UnresolvedComponentReference) ==
         "UnresolvedComponentReference");
  assert(utils::error_code_to_string(
             utils::ErrorCode::CircularWorkflowDependency) ==
         "CircularWorkflowDependency");
  assert(
      utils::error_code_to_string(utils::ErrorCode::WorkflowExecutionFailed) ==
      "WorkflowExecutionFailed");

  assert(to_string(SourceDescriptionType::OpenApi) == "openapi");
  assert(to_string(SourceDescriptionType::Arazzo) == "arazzo");
  assert(source_description_type_from_string("openapi") ==
         SourceDescriptionType::OpenApi);
  assert(source_description_type_from_string("arazzo") ==
         SourceDescriptionType::Arazzo);
  assert(!source_description_type_from_string("invalid").has_value());

  assert(to_string(ParameterIn::Path) == "path");
  assert(to_string(ParameterIn::Query) == "query");
  assert(to_string(ParameterIn::Header) == "header");
  assert(to_string(ParameterIn::Cookie) == "cookie");
  assert(parameter_in_from_string("path") == ParameterIn::Path);
  assert(parameter_in_from_string("query") == ParameterIn::Query);
  assert(parameter_in_from_string("header") == ParameterIn::Header);
  assert(parameter_in_from_string("cookie") == ParameterIn::Cookie);
  assert(!parameter_in_from_string("body").has_value());

  assert(to_string(CriterionType::Simple) == "simple");
  assert(to_string(CriterionType::Regex) == "regex");
  assert(to_string(CriterionType::JsonPath) == "jsonpath");
  assert(to_string(CriterionType::XPath) == "xpath");
  assert(criterion_type_from_string("simple") == CriterionType::Simple);
  assert(criterion_type_from_string("regex") == CriterionType::Regex);
  assert(criterion_type_from_string("jsonpath") == CriterionType::JsonPath);
  assert(criterion_type_from_string("xpath") == CriterionType::XPath);
  assert(!criterion_type_from_string("sql").has_value());

  assert(to_string(
             CriterionExpressionTypeVersion::DraftGoessnerDispatchJsonpath00) ==
         "draft-goessner-dispatch-jsonpath-00");
  assert(to_string(CriterionExpressionTypeVersion::Rfc9535) == "rfc9535");
  assert(to_string(CriterionExpressionTypeVersion::XPath30) == "xpath-30");
  assert(to_string(CriterionExpressionTypeVersion::XPath20) == "xpath-20");
  assert(to_string(CriterionExpressionTypeVersion::XPath10) == "xpath-10");
  assert(criterion_version_from_string("draft-goessner-dispatch-jsonpath-00") ==
         CriterionExpressionTypeVersion::DraftGoessnerDispatchJsonpath00);
  assert(criterion_version_from_string("rfc9535") ==
         CriterionExpressionTypeVersion::Rfc9535);
  assert(criterion_version_from_string("xpath-30") ==
         CriterionExpressionTypeVersion::XPath30);
  assert(criterion_version_from_string("xpath-20") ==
         CriterionExpressionTypeVersion::XPath20);
  assert(criterion_version_from_string("xpath-10") ==
         CriterionExpressionTypeVersion::XPath10);
  assert(!criterion_version_from_string("unknown").has_value());

  assert(to_string(SuccessActionType::End) == "end");
  assert(to_string(SuccessActionType::Goto) == "goto");
  assert(success_action_type_from_string("end") == SuccessActionType::End);
  assert(success_action_type_from_string("goto") == SuccessActionType::Goto);
  assert(!success_action_type_from_string("retry").has_value());

  assert(to_string(FailureActionType::End) == "end");
  assert(to_string(FailureActionType::Retry) == "retry");
  assert(to_string(FailureActionType::Goto) == "goto");
  assert(failure_action_type_from_string("end") == FailureActionType::End);
  assert(failure_action_type_from_string("retry") == FailureActionType::Retry);
  assert(failure_action_type_from_string("goto") == FailureActionType::Goto);
  assert(!failure_action_type_from_string("ignore").has_value());
}

static void test_arazzo_expressions() noexcept {
  assert(parse_runtime_expression("$url")->kind == RuntimeExpressionKind::Url);
  assert(parse_runtime_expression("$method")->kind ==
         RuntimeExpressionKind::Method);
  assert(parse_runtime_expression("$statusCode")->kind ==
         RuntimeExpressionKind::StatusCode);
  assert(parse_runtime_expression("$self")->kind ==
         RuntimeExpressionKind::Self);

  assert(parse_runtime_expression("$request.header.X-Token")->primary_key ==
         "X-Token");
  assert(!parse_runtime_expression("$request.header.").has_value());

  assert(parse_runtime_expression("$request.query.filter")->primary_key ==
         "filter");
  assert(!parse_runtime_expression("$request.query.").has_value());

  assert(parse_runtime_expression("$request.path.petId")->primary_key ==
         "petId");
  assert(!parse_runtime_expression("$request.path.").has_value());

  assert(parse_runtime_expression("$request.body")->kind ==
         RuntimeExpressionKind::RequestBody);
  assert(parse_runtime_expression("$request.body#/user/name")->json_pointer ==
         "/user/name");
  assert(!parse_runtime_expression("$request.body#bad").has_value());
  assert(!parse_runtime_expression("$request.body#/~2").has_value());

  assert(parse_runtime_expression("$response.header.ETag")->primary_key ==
         "ETag");
  assert(!parse_runtime_expression("$response.header.").has_value());

  assert(parse_runtime_expression("$response.query.page")->primary_key ==
         "page");
  assert(!parse_runtime_expression("$response.query.").has_value());

  assert(parse_runtime_expression("$response.path.id")->primary_key == "id");
  assert(!parse_runtime_expression("$response.path.").has_value());

  assert(parse_runtime_expression("$response.body")->kind ==
         RuntimeExpressionKind::ResponseBody);
  assert(
      parse_runtime_expression("$response.body#/items/0/~0/~1")->json_pointer ==
      "/items/0/~0/~1");
  assert(!parse_runtime_expression("$response.body#/~").has_value());

  assert(parse_runtime_expression("$inputs.username")->primary_key ==
         "username");
  assert(parse_runtime_expression("$inputs.user#/id")->json_pointer == "/id");
  assert(parse_runtime_expression("$inputs.user.profile.id")->json_pointer ==
         "/profile/id");
  assert(!parse_runtime_expression("$inputs.").has_value());
  assert(!parse_runtime_expression("$inputs.user.").has_value());
  assert(!parse_runtime_expression("$inputs.#/a").has_value());

  assert(parse_runtime_expression("$outputs.token")->primary_key == "token");
  assert(!parse_runtime_expression("$outputs.").has_value());

  auto step_expr = parse_runtime_expression("$steps.login.outputs.token#/jwt");
  assert(step_expr.has_value());
  assert(step_expr->primary_key == "login");
  assert(step_expr->secondary_key == "token");
  assert(step_expr->json_pointer == "/jwt");
  assert(!parse_runtime_expression("$steps.login").has_value());
  assert(!parse_runtime_expression("$steps..outputs.a").has_value());
  assert(!parse_runtime_expression("$steps.login.outputs.").has_value());

  auto wf_expr =
      parse_runtime_expression("$workflows.auth.outputs.session#/id");
  assert(wf_expr.has_value());
  assert(wf_expr->primary_key == "auth");
  assert(wf_expr->secondary_key == "session");
  assert(!parse_runtime_expression("$workflows.auth").has_value());
  assert(!parse_runtime_expression("$workflows.auth.outputs.").has_value());

  auto sd_expr = parse_runtime_expression("$sourceDescriptions.petstore.url");
  assert(sd_expr.has_value());
  assert(sd_expr->primary_key == "petstore");
  assert(sd_expr->secondary_key == "url");
  assert(!parse_runtime_expression("$sourceDescriptions.petstore").has_value());
  assert(
      !parse_runtime_expression("$sourceDescriptions.petstore.").has_value());

  assert(parse_runtime_expression("$components.parameters.petId")->kind ==
         RuntimeExpressionKind::ComponentsParameters);
  assert(!parse_runtime_expression("$components.parameters.").has_value());

  assert(parse_runtime_expression("$components.successActions.endOk")->kind ==
         RuntimeExpressionKind::ComponentsSuccessActions);
  assert(!parse_runtime_expression("$components.successActions.").has_value());

  assert(parse_runtime_expression("$components.failureActions.retryIt")->kind ==
         RuntimeExpressionKind::ComponentsFailureActions);
  assert(!parse_runtime_expression("$components.failureActions.").has_value());

  assert(parse_runtime_expression("$components.inputs.userIn")->kind ==
         RuntimeExpressionKind::ComponentsInputs);
  assert(!parse_runtime_expression("$components.inputs.").has_value());

  assert(!parse_runtime_expression("").has_value());
  assert(!parse_runtime_expression("not_an_expr").has_value());
  assert(!parse_runtime_expression("$unknown.thing").has_value());

  // Template extraction
  auto tmpl_ok = extract_template_expressions(
      "{$sourceDescriptions.petstore.url}#/paths/~1pet/{non_expr}");
  assert(tmpl_ok.has_value() && tmpl_ok->size() == 1);
  assert(!extract_template_expressions("{$unclosed").has_value());
  assert(!extract_template_expressions("{$unknown.bad}").has_value());
  assert(!extract_template_expressions("$unknown.bad").has_value());

  // Evaluation
  WorkflowExecutionContext ctx;
  ctx.url = "https://api.example.com/pets";
  ctx.method = "POST";
  ctx.status_code = 201;
  ctx.self_uri = "file:///arazzo.json";
  ctx.request_headers["Authorization"] = "Bearer abc";
  ctx.request_query["limit"] = "10";
  ctx.request_path["petId"] = "42";

  std::map<std::string, openapi::JsonValue> req_obj;
  req_obj["name"] = openapi::JsonValue("Fluffy");
  ctx.request_body = openapi::JsonValue(req_obj);

  ctx.response_headers["X-Rate"] = "100";
  ctx.response_query["q"] = "1";
  ctx.response_path["p"] = "2";

  std::map<std::string, openapi::JsonValue> resp_obj;
  resp_obj["id"] = openapi::JsonValue(int64_t{99});
  resp_obj["active"] = openapi::JsonValue(true);
  resp_obj["score"] = openapi::JsonValue(9.5);
  resp_obj["empty"] = openapi::JsonValue(nullptr);
  resp_obj["a/b~c"] = openapi::JsonValue("escaped");
  std::vector<openapi::JsonValue> arr_items = {openapi::JsonValue("first"),
                                               openapi::JsonValue("second")};
  resp_obj["tags"] = openapi::JsonValue(arr_items);
  ctx.response_body = openapi::JsonValue(resp_obj);

  ctx.inputs["apiKey"] = openapi::JsonValue("secret");
  ctx.outputs["finalId"] = openapi::JsonValue("99");
  ctx.step_outputs["s1"]["tok"] = openapi::JsonValue("xyz");
  ctx.workflow_outputs["wf0"]["res"] = openapi::JsonValue("ok");
  ctx.source_description_urls["petstore"] = "https://petstore.swagger.io/v2";
  ctx.component_parameters["sharedParam"] = openapi::JsonValue("sharedVal");

  assert(interpolate_template("$url", ctx).value() ==
         "https://api.example.com/pets");
  assert(interpolate_template("$method", ctx).value() == "POST");
  assert(interpolate_template("$statusCode", ctx).value() == "201");
  assert(interpolate_template("$self", ctx).value() == "file:///arazzo.json");
  assert(interpolate_template("$request.header.Authorization", ctx).value() ==
         "Bearer abc");
  assert(!interpolate_template("$request.header.Missing", ctx).has_value());
  assert(interpolate_template("$request.query.limit", ctx).value() == "10");
  assert(!interpolate_template("$request.query.Missing", ctx).has_value());
  assert(interpolate_template("$request.path.petId", ctx).value() == "42");
  assert(!interpolate_template("$request.path.Missing", ctx).has_value());
  assert(interpolate_template("$request.body#/name", ctx).value() == "Fluffy");

  assert(interpolate_template("$response.header.X-Rate", ctx).value() == "100");
  assert(!interpolate_template("$response.header.Missing", ctx).has_value());
  assert(interpolate_template("$response.query.q", ctx).value() == "1");
  assert(!interpolate_template("$response.query.Missing", ctx).has_value());
  assert(interpolate_template("$response.path.p", ctx).value() == "2");
  assert(!interpolate_template("$response.path.Missing", ctx).has_value());

  assert(interpolate_template("$response.body#/id", ctx).value() == "99");
  assert(interpolate_template("$response.body#/active", ctx).value() == "true");
  assert(interpolate_template("$response.body#/score", ctx).value() == "9.5");
  assert(interpolate_template("$response.body#/empty", ctx).value() == "null");
  assert(interpolate_template("$response.body#/a~1b~0c", ctx).value() ==
         "escaped");
  assert(interpolate_template("$response.body#/tags/1", ctx).value() ==
         "second");

  assert(!evaluate_json_pointer(ctx.response_body, "bad_no_slash").has_value());
  assert(!evaluate_json_pointer(ctx.response_body, "/missing_key").has_value());
  assert(!evaluate_json_pointer(ctx.response_body, "/tags/99").has_value());
  assert(!evaluate_json_pointer(ctx.response_body, "/id/sub").has_value());
  assert(evaluate_json_pointer(ctx.response_body, "#").has_value());

  assert(interpolate_template("$inputs.apiKey", ctx).value() == "secret");
  assert(!interpolate_template("$inputs.missing", ctx).has_value());
  assert(interpolate_template("$outputs.finalId", ctx).value() == "99");
  assert(!interpolate_template("$outputs.missing", ctx).has_value());
  assert(interpolate_template("$steps.s1.outputs.tok", ctx).value() == "xyz");
  assert(!interpolate_template("$steps.missing.outputs.tok", ctx).has_value());
  assert(!interpolate_template("$steps.s1.outputs.missing", ctx).has_value());
  assert(interpolate_template("$workflows.wf0.outputs.res", ctx).value() ==
         "ok");
  assert(
      !interpolate_template("$workflows.missing.outputs.res", ctx).has_value());
  assert(
      !interpolate_template("$workflows.wf0.outputs.missing", ctx).has_value());
  assert(
      interpolate_template("$sourceDescriptions.petstore.url", ctx).value() ==
      "https://petstore.swagger.io/v2");
  assert(!interpolate_template("$sourceDescriptions.missing.url", ctx)
              .has_value());
  assert(
      interpolate_template("$components.parameters.sharedParam", ctx).value() ==
      "sharedVal");
  assert(
      !interpolate_template("$components.parameters.missing", ctx).has_value());
  assert(interpolate_template("$components.successActions.act1", ctx).value() ==
         "act1");

  assert(interpolate_template("Token: {$steps.s1.outputs.tok} {literal}", ctx)
             .value() == "Token: xyz {literal}");
  assert(!interpolate_template("Unclosed {$steps.s1.outputs.tok", ctx)
              .has_value());
  assert(!interpolate_template("Bad {$unknown}", ctx).has_value());
  assert(!interpolate_template("Missing {$inputs.nope}", ctx).has_value());

  // Payload replacement
  PayloadReplacement rep1;
  rep1.target = "#/name";
  rep1.value = openapi::JsonValue("$inputs.apiKey");
  auto replaced1 = apply_payload_replacement(ctx.request_body, rep1, ctx);
  assert(replaced1.has_value());

  PayloadReplacement rep2;
  rep2.target = "/tags/0";
  rep2.value = openapi::JsonValue("Bearer {$steps.s1.outputs.tok}");
  auto replaced2 = apply_payload_replacement(ctx.response_body, rep2, ctx);
  assert(replaced2.has_value());

  PayloadReplacement rep_bad_ptr;
  rep_bad_ptr.target = "invalid_pointer";
  assert(!apply_payload_replacement(ctx.request_body, rep_bad_ptr, ctx)
              .has_value());

  PayloadReplacement rep_bad_arr;
  rep_bad_arr.target = "/tags/99";
  rep_bad_arr.value = openapi::JsonValue("x");
  assert(!apply_payload_replacement(ctx.response_body, rep_bad_arr, ctx)
              .has_value());

  PayloadReplacement rep_scalar;
  rep_scalar.target = "/id/nested";
  rep_scalar.value = openapi::JsonValue("x");
  assert(!apply_payload_replacement(ctx.response_body, rep_scalar, ctx)
              .has_value());

  // Criteria evaluation
  Criterion c_eq;
  c_eq.condition = "$statusCode == 201";
  assert(evaluate_criterion(c_eq, ctx).value() == true);

  Criterion c_neq;
  c_neq.condition = "$statusCode != 404";
  assert(evaluate_criterion(c_neq, ctx).value() == true);

  Criterion c_ge;
  c_ge.condition = "$statusCode >= 200";
  assert(evaluate_criterion(c_ge, ctx).value() == true);

  Criterion c_le;
  c_le.condition = "$statusCode <= 299";
  assert(evaluate_criterion(c_le, ctx).value() == true);

  Criterion c_gt;
  c_gt.condition = "$statusCode > 200";
  assert(evaluate_criterion(c_gt, ctx).value() == true);

  Criterion c_lt;
  c_lt.condition = "$statusCode < 300";
  assert(evaluate_criterion(c_lt, ctx).value() == true);

  Criterion c_str_cmp;
  c_str_cmp.context = "$method";
  c_str_cmp.condition = "$ == 'POST'";
  assert(evaluate_criterion(c_str_cmp, ctx).value() == true);

  Criterion c_truthy;
  c_truthy.condition = "$response.body#/active";
  assert(evaluate_criterion(c_truthy, ctx).value() == true);

  Criterion c_regex;
  c_regex.context = "$url";
  c_regex.condition = "^https?://[_ab-z0-9.]+\\.com/p.ts$";
  c_regex.type = CriterionType::Regex;
  assert(evaluate_criterion(c_regex, ctx).value() == true);

  Criterion c_regex_space_digit;
  c_regex_space_digit.context = "$request.header.Authorization";
  c_regex_space_digit.condition = "^Bearer\\s+\\w+$";
  c_regex_space_digit.type = CriterionType::Regex;
  assert(evaluate_criterion(c_regex_space_digit, ctx).value() == true);

  Criterion c_regex_digits;
  c_regex_digits.context = "$statusCode";
  c_regex_digits.condition = "^\\d+$";
  c_regex_digits.type = CriterionType::Regex;
  assert(evaluate_criterion(c_regex_digits, ctx).value() == true);

  Criterion c_regex_alt;
  c_regex_alt.context = "$method";
  c_regex_alt.condition = "^GET|O*ST|^OTHER$";
  c_regex_alt.type = CriterionType::Regex;
  assert(evaluate_criterion(c_regex_alt, ctx).value() == true);

  Criterion c_regex_alt_fail;
  c_regex_alt_fail.context = "$url";
  c_regex_alt_fail.condition =
      "^https$|^ht?X$|^https://api\\.example\\.com/pets_extra$";
  c_regex_alt_fail.type = CriterionType::Regex;
  assert(evaluate_criterion(c_regex_alt_fail, ctx).value() == false);

  Criterion c_regex_neg;
  c_regex_neg.context = "$response.body#/a~1b~0c";
  c_regex_neg.condition = "^[^0-9]+\\w+_[X]?$";
  c_regex_neg.type = CriterionType::Regex;
  ctx.response_body =
      openapi::JsonValue(std::map<std::string, openapi::JsonValue>{
          {"id", openapi::JsonValue(99)},
          {"active", openapi::JsonValue(true)},
          {"score", openapi::JsonValue(9.5)},
          {"empty", openapi::JsonValue(nullptr)},
          {"a/b~c", openapi::JsonValue("esc_aped~")},
          {"tags",
           openapi::JsonValue(std::vector<openapi::JsonValue>{
               openapi::JsonValue("first"), openapi::JsonValue("second")})}});
  assert(evaluate_criterion(c_regex_neg, ctx).value() == false);

  Criterion c_regex_word_underscore;
  c_regex_word_underscore.context = "$response.body#/a~1b~0c";
  c_regex_word_underscore.condition = "^[a-z]+_\\w+~$";
  c_regex_word_underscore.type = CriterionType::Regex;
  assert(evaluate_criterion(c_regex_word_underscore, ctx).value() == true);

  Criterion c_regex_plus_fail;
  c_regex_plus_fail.context = "$method";
  c_regex_plus_fail.condition = "\\d+";
  c_regex_plus_fail.type = CriterionType::Regex;
  assert(evaluate_criterion(c_regex_plus_fail, ctx).value() == false);

  Criterion c_and_ok;
  c_and_ok.condition = "$statusCode == 201 && $method == 'POST'";
  assert(evaluate_criterion(c_and_ok, ctx).value() == true);

  Criterion c_and_short;
  c_and_short.condition = "$statusCode == 404 && $method == 'POST'";
  assert(evaluate_criterion(c_and_short, ctx).value() == false);

  Criterion c_and_err;
  c_and_err.condition = "$invalid_expr && $statusCode == 201";
  assert(!evaluate_criterion(c_and_err, ctx).has_value());

  Criterion c_or_short;
  c_or_short.condition = "$statusCode == 201 || $statusCode == 200";
  assert(evaluate_criterion(c_or_short, ctx).value() == true);

  Criterion c_or_second;
  c_or_second.condition = "$statusCode == 404 || $statusCode == 201";
  assert(evaluate_criterion(c_or_second, ctx).value() == true);

  Criterion c_or_err;
  c_or_err.condition = "$invalid_expr || $statusCode == 201";
  assert(!evaluate_criterion(c_or_err, ctx).has_value());

  Criterion c_jp;
  c_jp.context = "$response.body";
  c_jp.condition = "$.id == 99";
  CriterionExpressionType cet;
  cet.type = CriterionType::JsonPath;
  cet.version = CriterionExpressionTypeVersion::Rfc9535;
  c_jp.type = cet;
  assert(evaluate_criterion(c_jp, ctx).value() == true);

  Criterion c_jp_bad;
  c_jp_bad.condition = "invalid_jp";
  c_jp_bad.type = CriterionType::JsonPath;
  assert(!evaluate_criterion(c_jp_bad, ctx).has_value());

  Criterion c_xpath;
  c_xpath.context = "$url";
  c_xpath.condition = "/root";
  c_xpath.type = CriterionType::XPath;
  assert(evaluate_criterion(c_xpath, ctx).value() == true);

  Criterion c_empty;
  c_empty.condition = " \t\n\r ";
  assert(!evaluate_criterion(c_empty, ctx).has_value());

  // Additional expression branch tests
  assert(evaluate_json_pointer(ctx.response_body, "").has_value());
  assert(!parse_runtime_expression("$inputs.user#/bad~2").has_value());
  assert(
      !parse_runtime_expression("$workflows.w.outputs.k#/bad~2").has_value());
  assert(parse_runtime_expression("$workflows.w.outputs.k")->secondary_key ==
         "k");
  assert(parse_runtime_expression("$steps.s.outputs.k")->secondary_key == "k");
  assert(parse_runtime_expression("$inputs.k")->primary_key == "k");
  assert(extract_template_expressions("no_braces_at_all")->empty());

  auto expr_no_ptr = parse_runtime_expression("$request.body");
  assert(evaluate_expression(*expr_no_ptr, ctx).has_value());
  auto expr_with_ptr = parse_runtime_expression("$request.body#/name");
  assert(evaluate_expression(*expr_with_ptr, ctx).has_value());

  assert(!interpolate_template("$unknown.bad", ctx).has_value());
  assert(!interpolate_template("$inputs.nonexistent", ctx).has_value());
  assert(interpolate_template("plain_text", ctx).value() == "plain_text");

  PayloadReplacement rep_empty_target;
  rep_empty_target.target = "";
  rep_empty_target.value = openapi::JsonValue(int64_t{42});
  assert(apply_payload_replacement(ctx.request_body, rep_empty_target, ctx)
             .has_value());

  PayloadReplacement rep_bad_expr;
  rep_bad_expr.target = "/name";
  rep_bad_expr.value = openapi::JsonValue("$inputs.missing_input");
  assert(!apply_payload_replacement(ctx.request_body, rep_bad_expr, ctx)
              .has_value());

  Criterion c_bad_ctx;
  c_bad_ctx.context = "$unknown.ctx";
  c_bad_ctx.condition = "$ == 1";
  assert(!evaluate_criterion(c_bad_ctx, ctx).has_value());

  Criterion c_missing_ctx;
  c_missing_ctx.context = "$inputs.missing";
  c_missing_ctx.condition = "$ == 1";
  assert(!evaluate_criterion(c_missing_ctx, ctx).has_value());

  Criterion c_double_quote;
  c_double_quote.context = "$method";
  c_double_quote.condition = "$ != \"GET\"";
  assert(evaluate_criterion(c_double_quote, ctx).value() == true);

  Criterion c_bad_lhs;
  c_bad_lhs.condition = "$inputs.missing == 1";
  assert(!evaluate_criterion(c_bad_lhs, ctx).has_value());

  Criterion c_bad_rhs;
  c_bad_rhs.condition = "1 == $inputs.missing";
  assert(!evaluate_criterion(c_bad_rhs, ctx).has_value());

  Criterion c_bad_single;
  c_bad_single.condition = "$inputs.missing";
  assert(!evaluate_criterion(c_bad_single, ctx).has_value());

  Criterion c_xpath_empty;
  c_xpath_empty.condition = "/root";
  c_xpath_empty.type = CriterionType::XPath;
  assert(evaluate_criterion(c_xpath_empty, ctx).value() == false);

  Criterion c_xpath_no_slash;
  c_xpath_no_slash.context = "$url";
  c_xpath_no_slash.condition = "root";
  c_xpath_no_slash.type = CriterionType::XPath;
  assert(evaluate_criterion(c_xpath_no_slash, ctx).value() == false);

  Criterion c_jp_end;
  c_jp_end.context = "$response.body";
  c_jp_end.condition = "$.id";
  c_jp_end.type = CriterionType::JsonPath;
  assert(evaluate_criterion(c_jp_end, ctx).value() == true);

  Criterion c_dollar_no_ctx;
  c_dollar_no_ctx.condition = "$ == ''";
  assert(evaluate_criterion(c_dollar_no_ctx, ctx).value() == true);
}

static void test_arazzo_parse_and_emit() noexcept {
  const std::string full_arazzo_json = R"({
    "arazzo": "1.1.0",
    "info": {
      "title": "Petstore Workflow",
      "summary": "Orchestrates petstore operations",
      "description": "End-to-end pet adoption workflow",
      "version": "1.0.0",
      "x-info-ext": "val"
    },
    "sourceDescriptions": [
      {
        "name": "petstore",
        "url": "https://petstore.swagger.io/v2/swagger.json",
        "type": "openapi",
        "x-sd-ext": 123
      }
    ],
    "workflows": [
      {
        "workflowId": "loginAndAdopt",
        "summary": "Login and adopt a pet",
        "description": "Multi-step workflow",
        "inputs": {
          "type": "object",
          "title": "LoginInputs",
          "description": "Input schema",
          "required": ["username"],
          "properties": {
            "username": { "type": "string", "format": "email", "default": "user@example.com" },
            "tags": { "type": "array", "items": { "type": "string" } }
          }
        },
        "dependsOn": ["initEnv"],
        "steps": [
          {
            "stepId": "loginStep",
            "description": "Authenticate user",
            "operationId": "loginUser",
            "parameters": [
              {
                "name": "username",
                "in": "query",
                "value": "$inputs.username",
                "x-param-ext": true
              },
              {
                "reference": "$components.parameters.apiKeyParam",
                "value": "overrideKey"
              }
            ],
            "requestBody": {
              "contentType": "application/json",
              "payload": {
                "user": "placeholder",
                "count": 1,
                "ratio": 1.5,
                "active": true,
                "list": [1, "two"],
                "nil": null
              },
              "replacements": [
                {
                  "target": "/user",
                  "value": "$inputs.username",
                  "x-rep-ext": "r"
                }
              ],
              "x-rb-ext": "rb"
            },
            "successCriteria": [
              {
                "context": "$statusCode",
                "condition": "$statusCode == 200",
                "type": "simple",
                "x-crit-ext": 1
              },
              {
                "context": "$response.body",
                "condition": "$.token",
                "type": {
                  "type": "jsonpath",
                  "version": "rfc9535",
                  "x-cet-ext": 2
                }
              }
            ],
            "onSuccess": [
              {
                "name": "goNext",
                "type": "goto",
                "stepId": "adoptStep",
                "criteria": [
                  { "condition": "$statusCode == 200" }
                ],
                "x-sa-ext": "s"
              },
              {
                "reference": "$components.successActions.sharedEnd"
              }
            ],
            "onFailure": [
              {
                "name": "retryLogin",
                "type": "retry",
                "stepId": "loginStep",
                "retryAfter": 1.5,
                "retryLimit": 3,
                "criteria": [
                  { "condition": "$statusCode == 503" }
                ],
                "x-fa-ext": "f"
              },
              {
                "reference": "$components.failureActions.sharedFail"
              }
            ],
            "outputs": {
              "token": "$response.body#/token"
            },
            "x-step-ext": "st"
          },
          {
            "stepId": "adoptStep",
            "operationPath": "{$sourceDescriptions.petstore.url}#/paths/~1pet/post"
          }
        ],
        "successActions": [
          { "name": "defaultEnd", "type": "end" }
        ],
        "failureActions": [
          { "name": "defaultFail", "type": "end" }
        ],
        "outputs": {
          "authToken": "$steps.loginStep.outputs.token"
        },
        "parameters": [
          { "name": "traceId", "in": "header", "value": "12345" }
        ],
        "x-wf-ext": "wf"
      },
      {
        "workflowId": "initEnv",
        "steps": [
          {
            "stepId": "checkHealth",
            "operationId": "ping"
          }
        ]
      }
    ],
    "components": {
      "inputs": {
        "SharedInput": { "type": "object" }
      },
      "parameters": {
        "apiKeyParam": {
          "name": "X-API-Key",
          "in": "header",
          "value": "defaultKey"
        }
      },
      "successActions": {
        "sharedEnd": {
          "name": "sharedEnd",
          "type": "end"
        }
      },
      "failureActions": {
        "sharedFail": {
          "name": "sharedFail",
          "type": "end"
        }
      },
      "x-comp-ext": "comp"
    },
    "x-root-ext": "root"
  })";

  auto parsed = parse(full_arazzo_json);
  assert(parsed.has_value());
  std::string emitted = emit(*parsed);
  auto reparsed = parse(emitted);
  assert(reparsed.has_value());
  assert(*parsed == *reparsed);

  // Error cases in parse()
  assert(!parse("{malformed").has_value());
  assert(!parse("[]").has_value());
  assert(!parse("{}").has_value());
  assert(!parse(R"({"arazzo":"9.9.9"})").has_value());
  assert(!parse(R"({"arazzo":"1.1.0"})").has_value());
  assert(!parse(R"({"arazzo":"1.1.0","info":{}})").has_value());
  assert(!parse(R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"}})")
              .has_value());
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[]})")
           .has_value());
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[123]})")
           .has_value());
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"bad name!","url":"u"}]})")
           .has_value());
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u","type":"bad"}]})")
           .has_value());
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u"}],"workflows":[]})")
           .has_value());
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u"}],"workflows":[{"workflowId":"w1","steps":[]}]})")
           .has_value());
  // Step without operationId/operationPath/workflowId
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u"}],"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1"}]}]})")
           .has_value());
  // Step with both operationId and workflowId
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u"}],"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","workflowId":"w2"}]}]})")
           .has_value());
  // Circular dependsOn
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u"}],"workflows":[{"workflowId":"w1","dependsOn":["w2"],"steps":[{"stepId":"s1","operationId":"op"}]},{"workflowId":"w2","dependsOn":["w1"],"steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());

  // Minimal Arazzo emit (no optional fields, no extensions, no components, SD
  // without type, Param without in, Reusable without value)
  Arazzo min_spec;
  min_spec.arazzo = "1.0.1";
  min_spec.info.title = "Min";
  min_spec.info.version = "1.0";
  SourceDescription min_sd;
  min_sd.name = "sd1";
  min_sd.url = "http://example.com";
  min_spec.sourceDescriptions.push_back(min_sd);
  Workflow min_wf;
  min_wf.workflowId = "wf1";
  Step min_step;
  min_step.stepId = "st1";
  min_step.workflowId = "wf1";
  Parameter param_no_in;
  param_no_in.name = "p1";
  param_no_in.value = openapi::JsonValue("lit");
  ReusableObject ro_no_val;
  ro_no_val.reference = "$components.parameters.p";
  min_step.parameters =
      std::vector<ParameterOrReusable>{param_no_in, ro_no_val};
  RequestBody min_rb;
  min_step.requestBody = min_rb;
  min_wf.steps.push_back(min_step);
  min_spec.workflows.push_back(min_wf);
  min_spec.components = Components{};
  std::string min_emitted = emit(min_spec);
  assert(parse(min_emitted).has_value());

  // Exhaustive parser error branch coverage
  const std::string base_prefix =
      R"({"arazzo":"1.0.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u"}],)";
  // Criterion errors
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[123]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[{}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[{"context":"$bad","condition":"1"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[{"condition":"1","type":"bad"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[{"condition":"1","type":{}}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[{"condition":"1","type":{"type":"bad","version":"rfc9535"}}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[{"condition":"1","type":{"type":"jsonpath","version":"bad"}}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","successCriteria":[{"condition":"1","type":123}]}]}]})")
           .has_value());

  // Parameter with non-string value (integer)
  assert(
      parse(
          base_prefix +
          R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{"name":"p","value":42}]}]}]})")
          .has_value());
  // Missing workflows array
  assert(
      !parse(
           R"({"arazzo":"1.1.0","info":{"title":"T","version":"1"},"sourceDescriptions":[{"name":"sd","url":"u"}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[123]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{"reference":"$bad"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{"reference":"$url"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{"name":"p","in":"bad","value":"v"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{"name":"p"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{"name":"p","value":"$bad"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","parameters":[{"name":"p","value":"{$bad}"}]}]}]})")
           .has_value());

  // SuccessAction errors
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onSuccess":[123]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onSuccess":[{"reference":"$url"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onSuccess":[{}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onSuccess":[{"name":"s","type":"bad"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onSuccess":[{"name":"s","type":"goto","workflowId":"w","stepId":"s"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onSuccess":[{"name":"s","type":"end","criteria":[123]}]}]}]})")
           .has_value());

  // FailureAction errors
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[123]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[{"reference":"$url"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[{}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[{"name":"f","type":"bad"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[{"name":"f","type":"goto","workflowId":"w","stepId":"s"}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[{"name":"f","type":"retry","retryAfter":-1.0}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[{"name":"f","type":"retry","retryLimit":-1}]}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","onFailure":[{"name":"f","type":"end","criteria":[123]}]}]}]})")
           .has_value());

  // Outputs errors
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","outputs":{"bad key!":"$url"}}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","outputs":{"k":123}}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","outputs":{"k":"$bad"}}]}]})")
           .has_value());

  // Step & RequestBody errors
  assert(!parse(base_prefix +
                R"("workflows":[{"workflowId":"w1","steps":[{"stepId":""}]}]})")
              .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","requestBody":{"replacements":[123]}}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op","requestBody":{"replacements":[{}]}}]}]})")
           .has_value());

  // Workflow errors
  assert(!parse(base_prefix + R"("workflows":[123]})").has_value());
  assert(!parse(base_prefix + R"("workflows":[{}]})").has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","dependsOn":[""],"steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
  assert(!parse(base_prefix + R"("workflows":[{"workflowId":"w1"}]})")
              .has_value());
  assert(!parse(base_prefix +
                R"("workflows":[{"workflowId":"w1","steps":[123]}]})")
              .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"},{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}],"successActions":[123]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}],"failureActions":[123]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}],"outputs":{"k":123}}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}],"parameters":[123]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]},{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());

  // Components errors
  assert(
      !parse(
           base_prefix +
           R"("components":{"parameters":{"p":123}},"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("components":{"parameters":{"p":{}}},"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("components":{"successActions":{"s":123}},"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("components":{"successActions":{"s":{}}},"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("components":{"failureActions":{"f":123}},"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
  assert(
      !parse(
           base_prefix +
           R"("components":{"failureActions":{"f":{}}},"workflows":[{"workflowId":"w1","steps":[{"stepId":"s1","operationId":"op"}]}]})")
           .has_value());
}

static void test_arazzo_generators_and_cli() noexcept {
  const std::string spec_json = R"({
    "arazzo": "1.1.0",
    "info": {
      "title": "Test Workflows",
      "summary": "Summary text",
      "description": "Description text",
      "version": "1.0.0"
    },
    "sourceDescriptions": [
      { "name": "api", "url": "./openapi.json", "type": "openapi" }
    ],
    "workflows": [
      {
        "workflowId": "createOrder",
        "summary": "Create order workflow",
        "description": "Runs order creation",
        "dependsOn": ["authWorkflow"],
        "inputs": {
          "type": "object",
          "properties": {
            "itemId": { "type": "integer" },
            "price": { "type": "number" },
            "express": { "type": "boolean" },
            "note": { "type": "string" }
          }
        },
        "steps": [
          { "stepId": "s1", "operationId": "postOrder" },
          { "stepId": "s2", "operationPath": "{$sourceDescriptions.api.url}#/paths/~1order/get" },
          { "stepId": "s3", "workflowId": "authWorkflow" }
        ],
        "outputs": {
          "orderId": "$response.body#/id"
        }
      },
      {
        "workflowId": "authWorkflow",
        "steps": [
          { "stepId": "login", "operationId": "loginOp" }
        ]
      }
    ]
  })";

  auto spec_res = parse(spec_json);
  assert(spec_res.has_value());
  const auto &spec = *spec_res;

  // Functions emit & parse
  assert(functions::emit_arazzo(Arazzo{}) == "{}");
  std::string fn_code = functions::emit_arazzo(spec);
  auto fn_parsed = functions::parse_arazzo(fn_code);
  assert(fn_parsed.has_value());
  assert(*fn_parsed == spec);
  assert(!functions::parse_arazzo("").has_value());
  auto fn_fallback = functions::parse_arazzo(
      "namespace cdd_functions { void myWf(int a, double b, bool c, "
      "std::string d); }");
  assert(fn_fallback.has_value() && !fn_fallback->workflows.empty());

  // Mocks emit & parse
  std::string mock_code = mocks::emit_arazzo(spec);
  auto mock_parsed = mocks::parse_arazzo(mock_code);
  assert(mock_parsed.has_value());
  assert(*mock_parsed == spec);
  assert(!mocks::parse_arazzo("").has_value());
  auto mock_fallback = mocks::parse_arazzo(
      "class MockWorkflows { MOCK_METHOD((std::expected<std::string, "
      "std::string>), rawWf, (const std::string&), (noexcept, override)); };");
  assert(mock_fallback.has_value() && !mock_fallback->workflows.empty());

  // Client SDK emit & parse
  auto sdk_files =
      client_sdk::emit_arazzo_client(spec, std::nullopt, false, false, true);
  assert(sdk_files.contains("src/client.hpp"));
  assert(sdk_files.contains("src/client.cpp"));
  assert(sdk_files.contains("tests/client_test.cpp"));
  auto sdk_parsed = client_sdk::parse_arazzo(sdk_files["src/client.hpp"]);
  assert(sdk_parsed.has_value());
  assert(*sdk_parsed == spec);
  assert(!client_sdk::parse_arazzo("").has_value());
  auto sdk_fallback = client_sdk::parse_arazzo(
      "namespace cdd_client { class Workflows { int dummy; }; void "
      "customWf() {} }");
  assert(sdk_fallback.has_value() && !sdk_fallback->workflows.empty());

  // Client SDK CLI emit & parse
  auto cli_files =
      client_sdk_cli::emit_arazzo_cli(spec, std::nullopt, false, false, true);
  assert(cli_files.contains("src/cli.hpp"));
  assert(cli_files.contains("src/cli.cpp"));
  auto cli_parsed = client_sdk_cli::parse_arazzo(cli_files["src/cli.hpp"]);
  assert(cli_parsed.has_value());
  assert(*cli_parsed == spec);
  assert(!client_sdk_cli::parse_arazzo("").has_value());
  auto cli_fallback = client_sdk_cli::parse_arazzo(
      "std::expected<std::string, std::string> handle_workflow_manualWf("
      "cdd_client::Workflows& w, const std::vector<std::string>& a);");
  assert(cli_fallback.has_value() && !cli_fallback->workflows.empty());

  // Server emit & parse
  auto srv_files = server::emit_arazzo_modular(spec, false, true);
  assert(srv_files.contains("src/WorkflowEngine.hpp"));
  assert(srv_files.contains("src/workflow_server.cpp"));
  assert(srv_files.contains("tests/WorkflowEngineTest.cpp"));
  auto srv_parsed = server::parse_arazzo(srv_files["src/WorkflowEngine.hpp"]);
  assert(srv_parsed.has_value());
  assert(*srv_parsed == spec);
  assert(!server::parse_arazzo("").has_value());
  auto srv_fallback = server::parse_arazzo(
      "class WorkflowEngine { void on_workflow_srvWf(WorkflowHandler h) {} };");
  assert(srv_fallback.has_value() && !srv_fallback->workflows.empty());

  // Directory scanner & CLI from_arazzo / to_arazzo
  std::string tmp_dir =
      (std::filesystem::temp_directory_path() / "cdd_arazzo_test_dir").string();
  std::error_code ec;
  std::filesystem::remove_all(tmp_dir, ec);
  std::filesystem::create_directories(tmp_dir, ec);

  std::string spec_file = tmp_dir + "/spec.arazzo.json";
  {
    std::ofstream ofs(spec_file);
    ofs << spec_json;
  }

  std::string out_sdk_dir = tmp_dir + "/sdk_out";
  {
    std::ostringstream out_ss, err_ss;
    const char *argv[] = {"cdd-cpp",
                          "from_arazzo",
                          "to_sdk",
                          "-i",
                          spec_file.c_str(),
                          "-o",
                          out_sdk_dir.c_str(),
                          "--tests",
                          "--no-github-actions",
                          "--no-installable-package"};
    assert(cli::main_impl(10, const_cast<char **>(argv), out_ss, err_ss) == 0);
  }
  {
    std::ostringstream out_ss, err_ss;
    std::string out_cli_dir = tmp_dir + "/cli_out";
    const char *argv[] = {
        "cdd-cpp",         "from_arazzo", "to_sdk_cli",       "-i",
        spec_file.c_str(), "-o",          out_cli_dir.c_str()};
    assert(cli::main_impl(7, const_cast<char **>(argv), out_ss, err_ss) == 0);
  }
  {
    std::ostringstream out_ss, err_ss;
    std::string out_srv_dir = tmp_dir + "/srv_out";
    const char *argv[] = {
        "cdd-cpp",         "from_arazzo", "to_server",        "-i",
        spec_file.c_str(), "-o",          out_srv_dir.c_str()};
    assert(cli::main_impl(7, const_cast<char **>(argv), out_ss, err_ss) == 0);
  }
  {
    std::ostringstream out_ss, err_ss;
    const char *argv[] = {"cdd-cpp", "from_arazzo", "--help"};
    assert(cli::main_impl(3, const_cast<char **>(argv), out_ss, err_ss) == 0);
  }
  {
    std::ostringstream out_ss, err_ss;
    const char *argv[] = {"cdd-cpp", "from_arazzo"};
    assert(cli::main_impl(2, const_cast<char **>(argv), out_ss, err_ss) == 1);
  }
  {
    std::ostringstream out_ss, err_ss;
    const char *argv[] = {"cdd-cpp", "to_arazzo", "--help"};
    assert(cli::main_impl(3, const_cast<char **>(argv), out_ss, err_ss) == 0);
  }
  {
    std::ostringstream out_ss, err_ss;
    std::string roundtrip_file = tmp_dir + "/roundtrip.arazzo.json";
    const char *argv[] = {"cdd-cpp", "to_arazzo",
                          "-i",      out_sdk_dir.c_str(),
                          "-o",      roundtrip_file.c_str()};
    assert(cli::main_impl(6, const_cast<char **>(argv), out_ss, err_ss) == 0);
  }
  {
    std::ostringstream out_ss, err_ss;
    const char *argv[] = {"cdd-cpp", "to_arazzo", "-i", out_sdk_dir.c_str()};
    assert(cli::main_impl(4, const_cast<char **>(argv), out_ss, err_ss) == 0);
  }

  auto fallback_proj =
      utils::parse_cpp_project_arazzo(tmp_dir + "/nonexistent_dir_xyz");
  assert(fallback_proj.arazzo == "1.1.0");

  // Additional CLI error paths & docstring fallback paths
  {
    Arazzo doc_spec;
    docstrings::parse_arazzo_docstrings(
        "/// @arazzo 1.0.1\n/// @arazzo_title T\n/// @arazzo_version 2.0\n/// "
        "@arazzo_spec_json {invalid}\n",
        doc_spec);
    assert(doc_spec.arazzo == "1.0.1");
    assert(doc_spec.info.title == "T");
    assert(doc_spec.info.version == "2.0");

    Workflow doc_wf;
    docstrings::parse_workflow_docstrings(
        "/// @workflowId wfX\n/// @summary Sum\n/// @description Desc\n",
        doc_wf);
    assert(doc_wf.workflowId == "wfX");
    assert(doc_wf.summary == "Sum");
    assert(doc_wf.description == "Desc");
  }

  // Test emit_arazzo_client & emit_arazzo_cli with explicit openapi_spec and
  // schema property without type
  {
    Arazzo custom_spec = spec;
    openapi::Schema untyped_prop;
    (*custom_spec.workflows[0].inputs->properties)["untypedField"] =
        untyped_prop;
    openapi::OpenAPI explicit_oa;
    explicit_oa.info.title = "CustomOA";
    explicit_oa.info.version = "1.0";
    auto sdk_no_test = client_sdk::emit_arazzo_client(custom_spec, explicit_oa,
                                                      true, true, false);
    assert(sdk_no_test.contains("src/client.hpp"));
    auto cli_with_oa = client_sdk_cli::emit_arazzo_cli(custom_spec, explicit_oa,
                                                       true, true, false);
    assert(cli_with_oa.contains("src/cli.hpp"));
    auto srv_no_pkg = server::emit_arazzo_modular(custom_spec, true, false);
    assert(!srv_no_pkg.contains("CMakeLists.txt"));

    std::string fn_untyped = functions::emit_arazzo(custom_spec);
    assert(!fn_untyped.empty());
  }

  // Test CLI error branches in from_arazzo and to_arazzo
  {
    std::ostringstream out_ss, err_ss;
    cli::FromArazzoConfig cfg;
    assert(cli::from_arazzo(cfg, out_ss, err_ss) == 1);

    cfg.input = tmp_dir + "/missing_file.json";
    assert(cli::from_arazzo(cfg, out_ss, err_ss) == 1);

    std::string bad_json_file = tmp_dir + "/bad.arazzo.json";
    {
      std::ofstream ofs(bad_json_file);
      ofs << "{bad";
    }
    cfg.input = bad_json_file;
    assert(cli::from_arazzo(cfg, out_ss, err_ss) == 1);

    cfg.input = spec_file;
    cfg.subcommand = "unknown_subcmd";
    assert(cli::from_arazzo(cfg, out_ss, err_ss) == 1);

    const char *argv_unknown_flag[] = {
        "cdd-cpp", "from_arazzo",     "to_sdk", "--unknown-flag",
        "-i",      spec_file.c_str(), "-o",     out_sdk_dir.c_str()};
    assert(cli::main_impl(8, const_cast<char **>(argv_unknown_flag), out_ss,
                          err_ss) == 0);

    cli::ToArazzoConfig to_cfg;
    assert(cli::to_arazzo(to_cfg, out_ss, err_ss) == 1);

    to_cfg.input = out_sdk_dir;
    to_cfg.output = tmp_dir + "/nonexistent_subdir_123/out.json";
    assert(cli::to_arazzo(to_cfg, out_ss, err_ss) == 1);

    const char *argv_to_unknown[] = {"cdd-cpp", "to_arazzo", "--unknown-flag",
                                     "-i", out_sdk_dir.c_str()};
    assert(cli::main_impl(5, const_cast<char **>(argv_to_unknown), out_ss,
                          err_ss) == 0);

    // JSON-RPC tool calls for from_arazzo and to_arazzo
    std::string rpc_from =
        R"({"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"cdd_generate","arguments":{"command":"from_arazzo","input":")" +
        spec_file + R"(","output":")" + out_sdk_dir + R"("}}})";
    assert(server::serve_json_rpc(rpc_from).find("OK") != std::string::npos);

    std::string rpc_to =
        R"({"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"cdd_generate","arguments":{"command":"to_arazzo","input":")" +
        out_sdk_dir + R"(","output":""}}})";
    assert(server::serve_json_rpc(rpc_to).find("OK") != std::string::npos);
  }

  // Add a fallback C++ file without @arazzo_spec_json and a non-cpp file +
  // subdirectory into out_sdk_dir
  {
    std::filesystem::create_directories(out_sdk_dir + "/subdir", ec);
    std::ofstream ofs_txt(out_sdk_dir + "/readme.txt");
    ofs_txt << "ignore";
    std::ofstream ofs_empty(out_sdk_dir + "/empty.cpp");
    std::ofstream ofs_fb(out_sdk_dir + "/fallback_wf.hpp");
    ofs_fb
        << "class WorkflowEngine { void on_workflow_extraSrvWf(int h) {} };\n";
    auto proj_res = utils::try_parse_cpp_project_arazzo(out_sdk_dir);
    assert(proj_res.has_value());
  }

  std::filesystem::remove_all(tmp_dir, ec);
}

void test_arazzo() noexcept {
  test_arazzo_enums_and_errors();
  test_arazzo_expressions();
  test_arazzo_parse_and_emit();
  test_arazzo_generators_and_cli();
}

} // namespace cdd_cpp::arazzo
