/// @file parse.cpp
/// @brief Implementation of Arazzo Specification 1.1.0 JSON parser without
/// exceptions (-fno-exceptions -fno-rtti).

#include "parse.hpp"
#include "expressions.hpp"
#include <set>
#include <simdjson.h>

namespace cdd_cpp::arazzo {

/// @brief Check if an identifier matches the Arazzo `[A-Za-z0-9_\-]+` pattern.
/// @param id The string view to check.
/// @return True if non-empty and all characters are alphanumeric, '_', or '-'.
static bool is_valid_arazzo_id(std::string_view id) noexcept {
  if (id.empty())
    return false;
  for (char c : id) {
    bool ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '-';
    if (!ok)
      return false;
  }
  return true;
}

/// @brief Convert a `simdjson::dom::element` into a strongly typed
/// `openapi::JsonValue`.
/// @param el The simdjson DOM element to convert.
/// @return Converted `openapi::JsonValue`.
static openapi::JsonValue parse_json_value(simdjson::dom::element el) noexcept {
  if (el.is_string()) {
    std::string_view sv;
    (void)el.get(sv);
    return openapi::JsonValue(std::string(sv));
  }
  if (el.is_int64()) {
    int64_t iv = 0;
    (void)el.get(iv);
    return openapi::JsonValue(iv);
  }
  if (el.is_double()) {
    double dv = 0.0;
    (void)el.get(dv);
    return openapi::JsonValue(dv);
  }
  if (el.is_bool()) {
    bool bv = false;
    (void)el.get(bv);
    return openapi::JsonValue(bv);
  }
  if (el.is_array()) {
    simdjson::dom::array arr;
    (void)el.get(arr);
    std::vector<openapi::JsonValue> vec;
    for (auto item : arr) {
      vec.push_back(parse_json_value(item));
    }
    return openapi::JsonValue(std::move(vec));
  }
  if (el.is_object()) {
    simdjson::dom::object obj;
    (void)el.get(obj);
    std::map<std::string, openapi::JsonValue> map_val;
    for (auto field : obj) {
      map_val[std::string(field.key)] = parse_json_value(field.value);
    }
    return openapi::JsonValue(std::move(map_val));
  }
  return openapi::JsonValue(nullptr);
}

/// @brief Extract specification extensions (`x-*`) from a DOM object.
/// @param obj The simdjson DOM object to inspect.
/// @return Optional ExtensionsMap if at least one `x-*` key is present.
static std::optional<ExtensionsMap>
parse_extensions(simdjson::dom::object obj) noexcept {
  ExtensionsMap ext;
  for (auto field : obj) {
    if (field.key.starts_with("x-")) {
      ext[std::string(field.key)] = parse_json_value(field.value);
    }
  }
  if (ext.empty())
    return std::nullopt;
  return ext;
}

/// @brief Extract an optional string field from a DOM object.
/// @param obj The simdjson DOM object.
/// @param key Property name.
/// @return Optional string value.
static std::optional<std::string> get_opt_str(simdjson::dom::object obj,
                                              std::string_view key) noexcept {
  std::string_view sv;
  if (obj[key].get(sv) == simdjson::SUCCESS) {
    return std::string(sv);
  }
  return std::nullopt;
}

/// @brief Parse a JSON Schema 2020-12 object for Arazzo `inputs`.
/// @param obj The simdjson DOM object representing the schema.
/// @return Parsed `openapi::Schema`.
static openapi::Schema parse_input_schema(simdjson::dom::object obj) noexcept {
  openapi::Schema s;
  s.type = get_opt_str(obj, "type");
  s.description = get_opt_str(obj, "description");
  s.format = get_opt_str(obj, "format");
  s.title = get_opt_str(obj, "title");
  s.default_value = get_opt_str(obj, "default");

  simdjson::dom::array req_arr;
  if (obj["required"].get(req_arr) == simdjson::SUCCESS) {
    std::vector<std::string> reqs;
    for (auto item : req_arr) {
      std::string_view sv;
      (void)item.get(sv);
      reqs.emplace_back(sv);
    }
    s.required = std::move(reqs);
  }

  simdjson::dom::object props_obj;
  if (obj["properties"].get(props_obj) == simdjson::SUCCESS) {
    auto props = std::make_shared<std::map<std::string, openapi::Schema>>();
    for (auto field : props_obj) {
      simdjson::dom::object prop_o;
      (void)field.value.get(prop_o);
      (*props)[std::string(field.key)] = parse_input_schema(prop_o);
    }
    s.properties = std::move(props);
  }

  simdjson::dom::object items_obj;
  if (obj["items"].get(items_obj) == simdjson::SUCCESS) {
    s.items = std::make_shared<openapi::Schema>(parse_input_schema(items_obj));
  }

  return s;
}

/// @brief Parse a Criterion Object from a DOM object.
/// @param obj The simdjson DOM object.
/// @return Result containing the parsed Criterion or a typed Error.
static Result<Criterion> parse_criterion(simdjson::dom::object obj) noexcept {
  Criterion crit;
  auto cond = get_opt_str(obj, "condition");
  if (!cond.has_value()) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Criterion missing required 'condition' field");
  }
  crit.condition = *cond;
  crit.context = get_opt_str(obj, "context");
  if (crit.context.has_value()) {
    auto expr_res = parse_runtime_expression(*crit.context);
    if (!expr_res.has_value()) {
      return std::unexpected(expr_res.error());
    }
  }

  simdjson::dom::element type_el;
  if (obj["type"].get(type_el) == simdjson::SUCCESS) {
    if (type_el.is_string()) {
      std::string_view t_sv;
      (void)type_el.get(t_sv);
      auto ct = criterion_type_from_string(t_sv);
      if (!ct.has_value()) {
        return utils::make_error(utils::ErrorCode::InvalidFieldType,
                                 "Invalid Criterion type: " +
                                     std::string(t_sv));
      }
      crit.type = *ct;
    } else if (type_el.is_object()) {
      simdjson::dom::object cet_obj;
      (void)type_el.get(cet_obj);
      auto t_str = get_opt_str(cet_obj, "type");
      auto v_str = get_opt_str(cet_obj, "version");
      if (!t_str.has_value()) {
        return utils::make_error(
            utils::ErrorCode::MissingRequiredField,
            "CriterionExpressionType requires 'type' and 'version'");
      }
      auto ct = criterion_type_from_string(*t_str);
      auto cv = criterion_version_from_string(v_str.value_or(""));
      if (!ct.has_value() || !cv.has_value()) {
        return utils::make_error(
            utils::ErrorCode::InvalidFieldType,
            "Invalid CriterionExpressionType type or version");
      }
      CriterionExpressionType cet;
      cet.type = *ct;
      cet.version = *cv;
      cet.extensions = parse_extensions(cet_obj);
      crit.type = std::move(cet);
    } else {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Criterion 'type' must be string or object");
    }
  }

  crit.extensions = parse_extensions(obj);
  return crit;
}

/// @brief Parse an optional array of Criterion Objects from a DOM object.
/// @param obj The parent simdjson DOM object.
/// @param key The array property key (`"criteria"` or `"successCriteria"`).
/// @return Result containing optional vector of Criterion or a typed Error.
static Result<std::optional<std::vector<Criterion>>>
parse_criteria_list(simdjson::dom::object obj, std::string_view key) noexcept {
  simdjson::dom::array arr;
  if (obj[key].get(arr) != simdjson::SUCCESS) {
    return std::optional<std::vector<Criterion>>{std::nullopt};
  }
  std::vector<Criterion> list;
  for (auto item : arr) {
    simdjson::dom::object c_obj;
    if (item.get(c_obj) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Criterion item must be an object");
    }
    auto c_res = parse_criterion(c_obj);
    if (!c_res.has_value()) {
      return std::unexpected(c_res.error());
    }
    list.push_back(std::move(*c_res));
  }
  return std::optional<std::vector<Criterion>>{std::move(list)};
}

/// @brief Parse a Parameter Object from a DOM object.
/// @param obj The simdjson DOM object.
/// @return Result containing the parsed Parameter or a typed Error.
static Result<Parameter> parse_parameter(simdjson::dom::object obj) noexcept {
  Parameter p;
  auto name = get_opt_str(obj, "name");
  if (!name.has_value()) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Parameter missing required 'name' field");
  }
  p.name = *name;

  auto in_str = get_opt_str(obj, "in");
  if (in_str.has_value()) {
    auto loc = parameter_in_from_string(*in_str);
    if (!loc.has_value()) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Invalid Parameter 'in' value: " + *in_str);
    }
    p.in = *loc;
  }

  simdjson::dom::element val_el;
  if (obj["value"].get(val_el) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Parameter missing required 'value' field");
  }
  p.value = parse_json_value(val_el);
  if (const auto *sv = std::get_if<std::string>(&p.value.data)) {
    auto expr_res = extract_template_expressions(*sv);
    if (!expr_res.has_value()) {
      return std::unexpected(expr_res.error());
    }
  }
  p.extensions = parse_extensions(obj);
  return p;
}

/// @brief Parse an array of Parameter or ReusableObject items.
/// @param obj The parent simdjson DOM object.
/// @param key Property key (`"parameters"`).
/// @return Result containing optional vector of ParameterOrReusable or Error.
static Result<std::optional<std::vector<ParameterOrReusable>>>
parse_parameters_or_reusables(simdjson::dom::object obj,
                              std::string_view key) noexcept {
  simdjson::dom::array arr;
  if (obj[key].get(arr) != simdjson::SUCCESS) {
    return std::optional<std::vector<ParameterOrReusable>>{std::nullopt};
  }
  std::vector<ParameterOrReusable> list;
  for (auto item : arr) {
    simdjson::dom::object p_obj;
    if (item.get(p_obj) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Parameter entry must be an object");
    }
    auto ref_str = get_opt_str(p_obj, "reference");
    if (ref_str.has_value()) {
      if (!ref_str->starts_with("$components.parameters.")) {
        return utils::make_error(
            utils::ErrorCode::InvalidRuntimeExpression,
            "Parameter ReusableObject must reference $components.parameters.*",
            *ref_str);
      }
      ReusableObject ro;
      ro.reference = *ref_str;
      simdjson::dom::element v_el;
      if (p_obj["value"].get(v_el) == simdjson::SUCCESS) {
        ro.value = parse_json_value(v_el);
      }
      list.emplace_back(std::move(ro));
    } else {
      auto p_res = parse_parameter(p_obj);
      if (!p_res.has_value()) {
        return std::unexpected(p_res.error());
      }
      list.emplace_back(std::move(*p_res));
    }
  }
  return std::optional<std::vector<ParameterOrReusable>>{std::move(list)};
}

/// @brief Parse a SuccessAction Object from a DOM object.
/// @param obj The simdjson DOM object.
/// @return Result containing the parsed SuccessAction or a typed Error.
static Result<SuccessAction>
parse_success_action(simdjson::dom::object obj) noexcept {
  SuccessAction sa;
  auto name = get_opt_str(obj, "name");
  auto type_str = get_opt_str(obj, "type");
  if (!name.has_value()) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "SuccessAction requires 'name' and 'type'");
  }
  auto st = success_action_type_from_string(type_str.value_or(""));
  if (!st.has_value()) {
    return utils::make_error(utils::ErrorCode::InvalidFieldType,
                             "Invalid SuccessAction type");
  }
  sa.name = *name;
  sa.type = *st;
  sa.workflowId = get_opt_str(obj, "workflowId");
  sa.stepId = get_opt_str(obj, "stepId");
  if (sa.workflowId.has_value()) {
    return utils::make_error(
        utils::ErrorCode::InvalidArazzoDocument,
        "SuccessAction cannot specify both workflowId and stepId");
  }
  auto crit_res = parse_criteria_list(obj, "criteria");
  if (!crit_res.has_value()) {
    return std::unexpected(crit_res.error());
  }
  sa.criteria = std::move(*crit_res);
  sa.extensions = parse_extensions(obj);
  return sa;
}

/// @brief Parse an array of SuccessAction or ReusableObject items.
/// @param obj The parent simdjson DOM object.
/// @param key Property key (`"onSuccess"` or `"successActions"`).
/// @return Result containing optional vector of SuccessActionOrReusable.
static Result<std::optional<std::vector<SuccessActionOrReusable>>>
parse_success_actions_or_reusables(simdjson::dom::object obj,
                                   std::string_view key) noexcept {
  simdjson::dom::array arr;
  if (obj[key].get(arr) != simdjson::SUCCESS) {
    return std::optional<std::vector<SuccessActionOrReusable>>{std::nullopt};
  }
  std::vector<SuccessActionOrReusable> list;
  for (auto item : arr) {
    simdjson::dom::object s_obj;
    if (item.get(s_obj) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "SuccessAction entry must be an object");
    }
    auto ref_str = get_opt_str(s_obj, "reference");
    if (ref_str.has_value()) {
      if (!ref_str->starts_with("$components.successActions.")) {
        return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                                 "SuccessAction ReusableObject must reference "
                                 "$components.successActions.*",
                                 *ref_str);
      }
      ReusableObject ro;
      ro.reference = *ref_str;
      list.emplace_back(std::move(ro));
    } else {
      auto sa_res = parse_success_action(s_obj);
      if (!sa_res.has_value()) {
        return std::unexpected(sa_res.error());
      }
      list.emplace_back(std::move(*sa_res));
    }
  }
  return std::optional<std::vector<SuccessActionOrReusable>>{std::move(list)};
}

/// @brief Parse a FailureAction Object from a DOM object.
/// @param obj The simdjson DOM object.
/// @return Result containing the parsed FailureAction or a typed Error.
static Result<FailureAction>
parse_failure_action(simdjson::dom::object obj) noexcept {
  FailureAction fa;
  auto name = get_opt_str(obj, "name");
  auto type_str = get_opt_str(obj, "type");
  if (!name.has_value()) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "FailureAction requires 'name' and 'type'");
  }
  auto ft = failure_action_type_from_string(type_str.value_or(""));
  if (!ft.has_value()) {
    return utils::make_error(utils::ErrorCode::InvalidFieldType,
                             "Invalid FailureAction type");
  }
  fa.name = *name;
  fa.type = *ft;
  fa.workflowId = get_opt_str(obj, "workflowId");
  fa.stepId = get_opt_str(obj, "stepId");
  if (fa.workflowId.has_value()) {
    return utils::make_error(
        utils::ErrorCode::InvalidArazzoDocument,
        "FailureAction cannot specify both workflowId and stepId");
  }

  double ra = 0.0;
  if (obj["retryAfter"].get(ra) == simdjson::SUCCESS) {
    if (ra < 0.0) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "FailureAction retryAfter must be non-negative");
    }
    fa.retryAfter = ra;
  }
  int64_t rl = 0;
  if (obj["retryLimit"].get(rl) == simdjson::SUCCESS) {
    if (rl < 0) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "FailureAction retryLimit must be non-negative");
    }
    fa.retryLimit = rl;
  }

  auto crit_res = parse_criteria_list(obj, "criteria");
  if (!crit_res.has_value()) {
    return std::unexpected(crit_res.error());
  }
  fa.criteria = std::move(*crit_res);
  fa.extensions = parse_extensions(obj);
  return fa;
}

/// @brief Parse an array of FailureAction or ReusableObject items.
/// @param obj The parent simdjson DOM object.
/// @param key Property key (`"onFailure"` or `"failureActions"`).
/// @return Result containing optional vector of FailureActionOrReusable.
static Result<std::optional<std::vector<FailureActionOrReusable>>>
parse_failure_actions_or_reusables(simdjson::dom::object obj,
                                   std::string_view key) noexcept {
  simdjson::dom::array arr;
  if (obj[key].get(arr) != simdjson::SUCCESS) {
    return std::optional<std::vector<FailureActionOrReusable>>{std::nullopt};
  }
  std::vector<FailureActionOrReusable> list;
  for (auto item : arr) {
    simdjson::dom::object f_obj;
    if (item.get(f_obj) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "FailureAction entry must be an object");
    }
    auto ref_str = get_opt_str(f_obj, "reference");
    if (ref_str.has_value()) {
      if (!ref_str->starts_with("$components.failureActions.")) {
        return utils::make_error(utils::ErrorCode::InvalidRuntimeExpression,
                                 "FailureAction ReusableObject must reference "
                                 "$components.failureActions.*",
                                 *ref_str);
      }
      ReusableObject ro;
      ro.reference = *ref_str;
      list.emplace_back(std::move(ro));
    } else {
      auto fa_res = parse_failure_action(f_obj);
      if (!fa_res.has_value()) {
        return std::unexpected(fa_res.error());
      }
      list.emplace_back(std::move(*fa_res));
    }
  }
  return std::optional<std::vector<FailureActionOrReusable>>{std::move(list)};
}

/// @brief Parse an outputs map (`map<string, string>`) and validate runtime
/// expressions.
/// @param obj The parent simdjson DOM object.
/// @return Result containing optional outputs map or Error.
static Result<std::optional<std::map<std::string, std::string>>>
parse_outputs_map(simdjson::dom::object obj) noexcept {
  simdjson::dom::object out_obj;
  if (obj["outputs"].get(out_obj) != simdjson::SUCCESS) {
    return std::optional<std::map<std::string, std::string>>{std::nullopt};
  }
  std::map<std::string, std::string> outs;
  for (auto field : out_obj) {
    if (!is_valid_arazzo_id(field.key)) {
      return utils::make_error(utils::ErrorCode::InvalidArazzoDocument,
                               "Invalid output key name: " +
                                   std::string(field.key));
    }
    std::string_view expr_sv;
    if (field.value.get(expr_sv) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Output expression must be a string");
    }
    auto expr_res = extract_template_expressions(expr_sv);
    if (!expr_res.has_value()) {
      return std::unexpected(expr_res.error());
    }
    outs[std::string(field.key)] = std::string(expr_sv);
  }
  return std::optional<std::map<std::string, std::string>>{std::move(outs)};
}

/// @brief Parse a Step Object from a DOM object.
/// @param obj The simdjson DOM object.
/// @return Result containing the parsed Step or a typed Error.
static Result<Step> parse_step(simdjson::dom::object obj) noexcept {
  Step step;
  auto sid = get_opt_str(obj, "stepId");
  if (!is_valid_arazzo_id(sid.value_or(""))) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Step missing or invalid 'stepId'");
  }
  step.stepId = *sid;
  step.description = get_opt_str(obj, "description");
  step.operationId = get_opt_str(obj, "operationId");
  step.operationPath = get_opt_str(obj, "operationPath");
  step.workflowId = get_opt_str(obj, "workflowId");

  int target_count = (step.operationId.has_value() ? 1 : 0) +
                     (step.operationPath.has_value() ? 1 : 0) +
                     (step.workflowId.has_value() ? 1 : 0);
  if (target_count != 1) {
    return utils::make_error(
        utils::ErrorCode::InvalidArazzoDocument,
        "Step must specify exactly one of operationId, operationPath, or "
        "workflowId",
        step.stepId);
  }

  auto params_res = parse_parameters_or_reusables(obj, "parameters");
  if (!params_res.has_value()) {
    return std::unexpected(params_res.error());
  }
  step.parameters = std::move(*params_res);

  simdjson::dom::object rb_obj;
  if (obj["requestBody"].get(rb_obj) == simdjson::SUCCESS) {
    RequestBody rb;
    rb.contentType = get_opt_str(rb_obj, "contentType");
    simdjson::dom::element pay_el;
    if (rb_obj["payload"].get(pay_el) == simdjson::SUCCESS) {
      rb.payload = parse_json_value(pay_el);
    }
    simdjson::dom::array rep_arr;
    if (rb_obj["replacements"].get(rep_arr) == simdjson::SUCCESS) {
      std::vector<PayloadReplacement> reps;
      for (auto r_item : rep_arr) {
        simdjson::dom::object r_obj;
        if (r_item.get(r_obj) != simdjson::SUCCESS) {
          return utils::make_error(utils::ErrorCode::InvalidFieldType,
                                   "PayloadReplacement must be an object");
        }
        auto target = get_opt_str(r_obj, "target");
        simdjson::dom::element rv_el;
        if (!target.has_value() ||
            r_obj["value"].get(rv_el) != simdjson::SUCCESS) {
          return utils::make_error(
              utils::ErrorCode::MissingRequiredField,
              "PayloadReplacement requires 'target' and 'value'");
        }
        PayloadReplacement pr;
        pr.target = *target;
        pr.value = parse_json_value(rv_el);
        pr.extensions = parse_extensions(r_obj);
        reps.push_back(std::move(pr));
      }
      rb.replacements = std::move(reps);
    }
    rb.extensions = parse_extensions(rb_obj);
    step.requestBody = std::move(rb);
  }

  auto sc_res = parse_criteria_list(obj, "successCriteria");
  if (!sc_res.has_value()) {
    return std::unexpected(sc_res.error());
  }
  step.successCriteria = std::move(*sc_res);

  auto os_res = parse_success_actions_or_reusables(obj, "onSuccess");
  if (!os_res.has_value()) {
    return std::unexpected(os_res.error());
  }
  step.onSuccess = std::move(*os_res);

  auto of_res = parse_failure_actions_or_reusables(obj, "onFailure");
  if (!of_res.has_value()) {
    return std::unexpected(of_res.error());
  }
  step.onFailure = std::move(*of_res);

  auto out_res = parse_outputs_map(obj);
  if (!out_res.has_value()) {
    return std::unexpected(out_res.error());
  }
  step.outputs = std::move(*out_res);
  step.extensions = parse_extensions(obj);
  return step;
}

/// @brief Parse a Workflow Object from a DOM object.
/// @param obj The simdjson DOM object.
/// @return Result containing the parsed Workflow or a typed Error.
static Result<Workflow> parse_workflow(simdjson::dom::object obj) noexcept {
  Workflow wf;
  auto wid = get_opt_str(obj, "workflowId");
  if (!is_valid_arazzo_id(wid.value_or(""))) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Workflow missing or invalid 'workflowId'");
  }
  wf.workflowId = *wid;
  wf.summary = get_opt_str(obj, "summary");
  wf.description = get_opt_str(obj, "description");

  simdjson::dom::object in_obj;
  if (obj["inputs"].get(in_obj) == simdjson::SUCCESS) {
    wf.inputs = parse_input_schema(in_obj);
  }

  simdjson::dom::array dep_arr;
  if (obj["dependsOn"].get(dep_arr) == simdjson::SUCCESS) {
    std::vector<std::string> deps;
    for (auto d_item : dep_arr) {
      std::string_view d_sv;
      (void)d_item.get(d_sv);
      if (d_sv.empty()) {
        return utils::make_error(utils::ErrorCode::InvalidFieldType,
                                 "dependsOn entry must be a non-empty string");
      }
      deps.emplace_back(d_sv);
    }
    wf.dependsOn = std::move(deps);
  }

  simdjson::dom::array steps_arr;
  if (obj["steps"].get(steps_arr) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Workflow missing required 'steps' array",
                             wf.workflowId);
  }
  std::set<std::string> step_ids;
  for (auto s_item : steps_arr) {
    simdjson::dom::object s_obj;
    if (s_item.get(s_obj) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Step item must be an object", wf.workflowId);
    }
    auto s_res = parse_step(s_obj);
    if (!s_res.has_value()) {
      return std::unexpected(s_res.error());
    }
    if (!step_ids.insert(s_res->stepId).second) {
      return utils::make_error(utils::ErrorCode::InvalidArazzoDocument,
                               "Duplicate stepId in workflow: " + s_res->stepId,
                               wf.workflowId);
    }
    wf.steps.push_back(std::move(*s_res));
  }
  if (wf.steps.empty()) {
    return utils::make_error(utils::ErrorCode::InvalidArazzoDocument,
                             "Workflow 'steps' array must not be empty",
                             wf.workflowId);
  }

  auto sa_res = parse_success_actions_or_reusables(obj, "successActions");
  if (!sa_res.has_value()) {
    return std::unexpected(sa_res.error());
  }
  wf.successActions = std::move(*sa_res);

  auto fa_res = parse_failure_actions_or_reusables(obj, "failureActions");
  if (!fa_res.has_value()) {
    return std::unexpected(fa_res.error());
  }
  wf.failureActions = std::move(*fa_res);

  auto out_res = parse_outputs_map(obj);
  if (!out_res.has_value()) {
    return std::unexpected(out_res.error());
  }
  wf.outputs = std::move(*out_res);

  auto params_res = parse_parameters_or_reusables(obj, "parameters");
  if (!params_res.has_value()) {
    return std::unexpected(params_res.error());
  }
  wf.parameters = std::move(*params_res);
  wf.extensions = parse_extensions(obj);
  return wf;
}

/// @brief Detect cycles in the workflow `dependsOn` graph using DFS coloring.
/// @param workflows The list of parsed workflows.
/// @return True if a cycle exists, false if the graph is a valid DAG.
static bool
has_workflow_cycle(const std::vector<Workflow> &workflows) noexcept {
  std::map<std::string, std::vector<std::string>> adj;
  for (const auto &wf : workflows) {
    adj[wf.workflowId] = wf.dependsOn.value_or(std::vector<std::string>{});
  }

  std::map<std::string, int> state; // 0 = unvisited, 1 = visiting, 2 = visited
  auto dfs = [&](auto &self, const std::string &node) noexcept -> bool {
    state[node] = 1;
    for (const auto &dep : adj[node]) {
      if (state[dep] == 1)
        return true;
      if (self(self, dep))
        return true;
    }
    state[node] = 2;
    return false;
  };

  for (const auto &wf : workflows) {
    if (state[wf.workflowId] == 0 && dfs(dfs, wf.workflowId)) {
      return true;
    }
  }
  return false;
}

Result<Arazzo> parse(const std::string &json_str) noexcept {
  simdjson::dom::parser parser;
  simdjson::dom::element doc;
  if (parser.parse(json_str).get(doc) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::JsonParseError,
                             "Failed to parse Arazzo JSON");
  }
  simdjson::dom::object root;
  if (doc.get(root) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::InvalidArazzoDocument,
                             "Arazzo document root must be a JSON object");
  }

  auto version_opt = get_opt_str(root, "arazzo");
  if (!version_opt.has_value()) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Missing required 'arazzo' version field");
  }
  if (*version_opt != "1.0.0" && *version_opt != "1.0.1" &&
      *version_opt != "1.1.0") {
    return utils::make_error(utils::ErrorCode::UnsupportedSpecVersion,
                             "Unsupported Arazzo version: " + *version_opt);
  }

  Arazzo spec;
  spec.arazzo = *version_opt;

  simdjson::dom::object info_obj;
  if (root["info"].get(info_obj) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Missing required 'info' object");
  }
  auto title = get_opt_str(info_obj, "title");
  auto ver = get_opt_str(info_obj, "version");
  if (!title.has_value()) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Info object requires 'title' and 'version'");
  }
  spec.info.title = *title;
  spec.info.summary = get_opt_str(info_obj, "summary");
  spec.info.description = get_opt_str(info_obj, "description");
  spec.info.version = ver.value_or("1.0.0");
  spec.info.extensions = parse_extensions(info_obj);

  simdjson::dom::array sd_arr;
  if (root["sourceDescriptions"].get(sd_arr) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Missing required 'sourceDescriptions' array");
  }
  for (auto item : sd_arr) {
    simdjson::dom::object sd_obj;
    if (item.get(sd_obj) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "SourceDescription item must be an object");
    }
    auto sd_name = get_opt_str(sd_obj, "name");
    auto sd_url = get_opt_str(sd_obj, "url");
    if (!is_valid_arazzo_id(sd_name.value_or(""))) {
      return utils::make_error(
          utils::ErrorCode::MissingRequiredField,
          "SourceDescription requires valid 'name' and non-empty 'url'");
    }
    SourceDescription sd;
    sd.name = *sd_name;
    sd.url = sd_url.value_or("");
    auto sd_type_str = get_opt_str(sd_obj, "type");
    if (sd_type_str.has_value()) {
      auto sdt = source_description_type_from_string(*sd_type_str);
      if (!sdt.has_value()) {
        return utils::make_error(utils::ErrorCode::InvalidFieldType,
                                 "Invalid SourceDescription type: " +
                                     *sd_type_str);
      }
      sd.type = *sdt;
    }
    sd.extensions = parse_extensions(sd_obj);
    spec.sourceDescriptions.push_back(std::move(sd));
  }
  if (spec.sourceDescriptions.empty()) {
    return utils::make_error(
        utils::ErrorCode::InvalidArazzoDocument,
        "'sourceDescriptions' array must contain at least one entry");
  }

  simdjson::dom::object comp_obj;
  if (root["components"].get(comp_obj) == simdjson::SUCCESS) {
    Components comp;
    simdjson::dom::object cin_obj;
    if (comp_obj["inputs"].get(cin_obj) == simdjson::SUCCESS) {
      std::map<std::string, openapi::Schema> m;
      for (auto field : cin_obj) {
        simdjson::dom::object s_obj;
        (void)field.value.get(s_obj);
        m[std::string(field.key)] = parse_input_schema(s_obj);
      }
      comp.inputs = std::move(m);
    }
    simdjson::dom::object cparam_obj;
    if (comp_obj["parameters"].get(cparam_obj) == simdjson::SUCCESS) {
      std::map<std::string, Parameter> m;
      for (auto field : cparam_obj) {
        simdjson::dom::object p_obj;
        if (field.value.get(p_obj) != simdjson::SUCCESS) {
          return utils::make_error(utils::ErrorCode::InvalidFieldType,
                                   "Component parameter must be an object");
        }
        auto p_res = parse_parameter(p_obj);
        if (!p_res.has_value()) {
          return std::unexpected(p_res.error());
        }
        m[std::string(field.key)] = std::move(*p_res);
      }
      comp.parameters = std::move(m);
    }
    simdjson::dom::object csa_obj;
    if (comp_obj["successActions"].get(csa_obj) == simdjson::SUCCESS) {
      std::map<std::string, SuccessAction> m;
      for (auto field : csa_obj) {
        simdjson::dom::object sa_obj;
        if (field.value.get(sa_obj) != simdjson::SUCCESS) {
          return utils::make_error(utils::ErrorCode::InvalidFieldType,
                                   "Component successAction must be an object");
        }
        auto sa_res = parse_success_action(sa_obj);
        if (!sa_res.has_value()) {
          return std::unexpected(sa_res.error());
        }
        m[std::string(field.key)] = std::move(*sa_res);
      }
      comp.successActions = std::move(m);
    }
    simdjson::dom::object cfa_obj;
    if (comp_obj["failureActions"].get(cfa_obj) == simdjson::SUCCESS) {
      std::map<std::string, FailureAction> m;
      for (auto field : cfa_obj) {
        simdjson::dom::object fa_obj;
        if (field.value.get(fa_obj) != simdjson::SUCCESS) {
          return utils::make_error(utils::ErrorCode::InvalidFieldType,
                                   "Component failureAction must be an object");
        }
        auto fa_res = parse_failure_action(fa_obj);
        if (!fa_res.has_value()) {
          return std::unexpected(fa_res.error());
        }
        m[std::string(field.key)] = std::move(*fa_res);
      }
      comp.failureActions = std::move(m);
    }
    comp.extensions = parse_extensions(comp_obj);
    spec.components = std::move(comp);
  }

  simdjson::dom::array wf_arr;
  if (root["workflows"].get(wf_arr) != simdjson::SUCCESS) {
    return utils::make_error(utils::ErrorCode::MissingRequiredField,
                             "Missing required 'workflows' array");
  }
  std::set<std::string> wf_ids;
  for (auto item : wf_arr) {
    simdjson::dom::object wf_obj;
    if (item.get(wf_obj) != simdjson::SUCCESS) {
      return utils::make_error(utils::ErrorCode::InvalidFieldType,
                               "Workflow item must be an object");
    }
    auto wf_res = parse_workflow(wf_obj);
    if (!wf_res.has_value()) {
      return std::unexpected(wf_res.error());
    }
    if (!wf_ids.insert(wf_res->workflowId).second) {
      return utils::make_error(utils::ErrorCode::InvalidArazzoDocument,
                               "Duplicate workflowId: " + wf_res->workflowId);
    }
    spec.workflows.push_back(std::move(*wf_res));
  }
  if (spec.workflows.empty()) {
    return utils::make_error(
        utils::ErrorCode::InvalidArazzoDocument,
        "'workflows' array must contain at least one workflow");
  }

  if (has_workflow_cycle(spec.workflows)) {
    return utils::make_error(
        utils::ErrorCode::CircularWorkflowDependency,
        "Cyclic workflow dependency detected in dependsOn");
  }

  spec.extensions = parse_extensions(root);
  return spec;
}

} // namespace cdd_cpp::arazzo
