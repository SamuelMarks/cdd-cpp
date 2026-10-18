/// @file emit.cpp
/// @brief Implementation of Arazzo Specification 1.1.0 JSON emitter.

#include "emit.hpp"

namespace cdd_cpp::arazzo {

void emit_json_value(utils::JsonWriter &jw,
                     const openapi::JsonValue &val) noexcept {
  if (const auto *s = std::get_if<std::string>(&val.data)) {
    jw.value(*s);
  } else if (const auto *i = std::get_if<int64_t>(&val.data)) {
    jw.raw_value(std::to_string(*i));
  } else if (const auto *d = std::get_if<double>(&val.data)) {
    jw.value(*d);
  } else if (const auto *b = std::get_if<bool>(&val.data)) {
    jw.value(*b);
  } else if (const auto *arr =
                 std::get_if<std::vector<openapi::JsonValue>>(&val.data)) {
    jw.start_array();
    for (const auto &item : *arr) {
      emit_json_value(jw, item);
    }
    jw.end_array();
  } else if (const auto *obj =
                 std::get_if<std::map<std::string, openapi::JsonValue>>(
                     &val.data)) {
    jw.start_object();
    for (const auto &[k, v] : *obj) {
      jw.key(k);
      emit_json_value(jw, v);
    }
    jw.end_object();
  } else {
    jw.null_value();
  }
}

/// @brief Write specification extensions (`x-*`) into the current JSON object.
/// @param jw Destination JsonWriter.
/// @param ext Optional ExtensionsMap.
static void emit_extensions(utils::JsonWriter &jw,
                            const std::optional<ExtensionsMap> &ext) noexcept {
  if (!ext.has_value())
    return;
  for (const auto &[k, v] : *ext) {
    jw.key(k);
    emit_json_value(jw, v);
  }
}

/// @brief Emit an Arazzo input JSON Schema 2020-12 object.
/// @param jw Destination JsonWriter.
/// @param s The Schema object to serialize.
static void emit_input_schema(utils::JsonWriter &jw,
                              const openapi::Schema &s) noexcept {
  jw.start_object();
  jw.key_optional("type", s.type);
  jw.key_optional("title", s.title);
  jw.key_optional("description", s.description);
  jw.key_optional("format", s.format);
  jw.key_optional("default", s.default_value);
  if (s.required.has_value()) {
    jw.key("required");
    jw.start_array();
    for (const auto &r : *s.required) {
      jw.value(r);
    }
    jw.end_array();
  }
  if (s.properties != nullptr) {
    jw.key("properties");
    jw.start_object();
    for (const auto &[k, prop] : *s.properties) {
      jw.key(k);
      emit_input_schema(jw, prop);
    }
    jw.end_object();
  }
  if (s.items != nullptr) {
    jw.key("items");
    emit_input_schema(jw, *s.items);
  }
  jw.end_object();
}

/// @brief Emit a Criterion Object.
/// @param jw Destination JsonWriter.
/// @param crit The Criterion object to serialize.
static void emit_criterion(utils::JsonWriter &jw,
                           const Criterion &crit) noexcept {
  jw.start_object();
  jw.key_optional("context", crit.context);
  jw.key_value("condition", crit.condition);
  if (crit.type.has_value()) {
    jw.key("type");
    if (const auto *ct = std::get_if<CriterionType>(&*crit.type)) {
      jw.value(std::string(to_string(*ct)));
    } else {
      const auto &cet = std::get<CriterionExpressionType>(*crit.type);
      jw.start_object();
      jw.key_value("type", std::string(to_string(cet.type)));
      jw.key_value("version", std::string(to_string(cet.version)));
      emit_extensions(jw, cet.extensions);
      jw.end_object();
    }
  }
  emit_extensions(jw, crit.extensions);
  jw.end_object();
}

/// @brief Emit a Parameter Object.
/// @param jw Destination JsonWriter.
/// @param p The Parameter object to serialize.
static void emit_parameter(utils::JsonWriter &jw, const Parameter &p) noexcept {
  jw.start_object();
  jw.key_value("name", p.name);
  if (p.in.has_value()) {
    jw.key_value("in", std::string(to_string(*p.in)));
  }
  jw.key("value");
  emit_json_value(jw, p.value);
  emit_extensions(jw, p.extensions);
  jw.end_object();
}

/// @brief Emit a ParameterOrReusable union item.
/// @param jw Destination JsonWriter.
/// @param item The ParameterOrReusable item to serialize.
static void
emit_parameter_or_reusable(utils::JsonWriter &jw,
                           const ParameterOrReusable &item) noexcept {
  if (const auto *p = std::get_if<Parameter>(&item)) {
    emit_parameter(jw, *p);
  } else {
    const auto &ro = std::get<ReusableObject>(item);
    jw.start_object();
    jw.key_value("reference", ro.reference);
    if (ro.value.has_value()) {
      jw.key("value");
      emit_json_value(jw, *ro.value);
    }
    jw.end_object();
  }
}

/// @brief Emit a SuccessAction Object.
/// @param jw Destination JsonWriter.
/// @param sa The SuccessAction object to serialize.
static void emit_success_action(utils::JsonWriter &jw,
                                const SuccessAction &sa) noexcept {
  jw.start_object();
  jw.key_value("name", sa.name);
  jw.key_value("type", std::string(to_string(sa.type)));
  jw.key_optional("workflowId", sa.workflowId);
  jw.key_optional("stepId", sa.stepId);
  if (sa.criteria.has_value()) {
    jw.key("criteria");
    jw.start_array();
    for (const auto &c : *sa.criteria) {
      emit_criterion(jw, c);
    }
    jw.end_array();
  }
  emit_extensions(jw, sa.extensions);
  jw.end_object();
}

/// @brief Emit a SuccessActionOrReusable union item.
/// @param jw Destination JsonWriter.
/// @param item The SuccessActionOrReusable item to serialize.
static void
emit_success_action_or_reusable(utils::JsonWriter &jw,
                                const SuccessActionOrReusable &item) noexcept {
  if (const auto *sa = std::get_if<SuccessAction>(&item)) {
    emit_success_action(jw, *sa);
  } else {
    const auto &ro = std::get<ReusableObject>(item);
    jw.start_object();
    jw.key_value("reference", ro.reference);
    jw.end_object();
  }
}

/// @brief Emit a FailureAction Object.
/// @param jw Destination JsonWriter.
/// @param fa The FailureAction object to serialize.
static void emit_failure_action(utils::JsonWriter &jw,
                                const FailureAction &fa) noexcept {
  jw.start_object();
  jw.key_value("name", fa.name);
  jw.key_value("type", std::string(to_string(fa.type)));
  jw.key_optional("workflowId", fa.workflowId);
  jw.key_optional("stepId", fa.stepId);
  if (fa.retryAfter.has_value()) {
    jw.key_value("retryAfter", *fa.retryAfter);
  }
  if (fa.retryLimit.has_value()) {
    jw.key("retryLimit");
    jw.raw_value(std::to_string(*fa.retryLimit));
  }
  if (fa.criteria.has_value()) {
    jw.key("criteria");
    jw.start_array();
    for (const auto &c : *fa.criteria) {
      emit_criterion(jw, c);
    }
    jw.end_array();
  }
  emit_extensions(jw, fa.extensions);
  jw.end_object();
}

/// @brief Emit a FailureActionOrReusable union item.
/// @param jw Destination JsonWriter.
/// @param item The FailureActionOrReusable item to serialize.
static void
emit_failure_action_or_reusable(utils::JsonWriter &jw,
                                const FailureActionOrReusable &item) noexcept {
  if (const auto *fa = std::get_if<FailureAction>(&item)) {
    emit_failure_action(jw, *fa);
  } else {
    const auto &ro = std::get<ReusableObject>(item);
    jw.start_object();
    jw.key_value("reference", ro.reference);
    jw.end_object();
  }
}

/// @brief Emit a Step Object.
/// @param jw Destination JsonWriter.
/// @param step The Step object to serialize.
static void emit_step(utils::JsonWriter &jw, const Step &step) noexcept {
  jw.start_object();
  jw.key_value("stepId", step.stepId);
  jw.key_optional("description", step.description);
  jw.key_optional("operationId", step.operationId);
  jw.key_optional("operationPath", step.operationPath);
  jw.key_optional("workflowId", step.workflowId);
  if (step.parameters.has_value()) {
    jw.key("parameters");
    jw.start_array();
    for (const auto &p : *step.parameters) {
      emit_parameter_or_reusable(jw, p);
    }
    jw.end_array();
  }
  if (step.requestBody.has_value()) {
    jw.key("requestBody");
    jw.start_object();
    jw.key_optional("contentType", step.requestBody->contentType);
    if (step.requestBody->payload.has_value()) {
      jw.key("payload");
      emit_json_value(jw, *step.requestBody->payload);
    }
    if (step.requestBody->replacements.has_value()) {
      jw.key("replacements");
      jw.start_array();
      for (const auto &rep : *step.requestBody->replacements) {
        jw.start_object();
        jw.key_value("target", rep.target);
        jw.key("value");
        emit_json_value(jw, rep.value);
        emit_extensions(jw, rep.extensions);
        jw.end_object();
      }
      jw.end_array();
    }
    emit_extensions(jw, step.requestBody->extensions);
    jw.end_object();
  }
  if (step.successCriteria.has_value()) {
    jw.key("successCriteria");
    jw.start_array();
    for (const auto &c : *step.successCriteria) {
      emit_criterion(jw, c);
    }
    jw.end_array();
  }
  if (step.onSuccess.has_value()) {
    jw.key("onSuccess");
    jw.start_array();
    for (const auto &sa : *step.onSuccess) {
      emit_success_action_or_reusable(jw, sa);
    }
    jw.end_array();
  }
  if (step.onFailure.has_value()) {
    jw.key("onFailure");
    jw.start_array();
    for (const auto &fa : *step.onFailure) {
      emit_failure_action_or_reusable(jw, fa);
    }
    jw.end_array();
  }
  if (step.outputs.has_value()) {
    jw.key("outputs");
    jw.start_object();
    for (const auto &[k, v] : *step.outputs) {
      jw.key_value(k, v);
    }
    jw.end_object();
  }
  emit_extensions(jw, step.extensions);
  jw.end_object();
}

/// @brief Emit a Workflow Object.
/// @param jw Destination JsonWriter.
/// @param wf The Workflow object to serialize.
static void emit_workflow(utils::JsonWriter &jw, const Workflow &wf) noexcept {
  jw.start_object();
  jw.key_value("workflowId", wf.workflowId);
  jw.key_optional("summary", wf.summary);
  jw.key_optional("description", wf.description);
  if (wf.inputs.has_value()) {
    jw.key("inputs");
    emit_input_schema(jw, *wf.inputs);
  }
  if (wf.dependsOn.has_value()) {
    jw.key("dependsOn");
    jw.start_array();
    for (const auto &d : *wf.dependsOn) {
      jw.value(d);
    }
    jw.end_array();
  }
  jw.key("steps");
  jw.start_array();
  for (const auto &s : wf.steps) {
    emit_step(jw, s);
  }
  jw.end_array();
  if (wf.successActions.has_value()) {
    jw.key("successActions");
    jw.start_array();
    for (const auto &sa : *wf.successActions) {
      emit_success_action_or_reusable(jw, sa);
    }
    jw.end_array();
  }
  if (wf.failureActions.has_value()) {
    jw.key("failureActions");
    jw.start_array();
    for (const auto &fa : *wf.failureActions) {
      emit_failure_action_or_reusable(jw, fa);
    }
    jw.end_array();
  }
  if (wf.outputs.has_value()) {
    jw.key("outputs");
    jw.start_object();
    for (const auto &[k, v] : *wf.outputs) {
      jw.key_value(k, v);
    }
    jw.end_object();
  }
  if (wf.parameters.has_value()) {
    jw.key("parameters");
    jw.start_array();
    for (const auto &p : *wf.parameters) {
      emit_parameter_or_reusable(jw, p);
    }
    jw.end_array();
  }
  emit_extensions(jw, wf.extensions);
  jw.end_object();
}

std::string emit(const Arazzo &spec) noexcept {
  utils::JsonWriter jw;
  jw.start_object();
  jw.key_value("arazzo", spec.arazzo);
  jw.key("info");
  jw.start_object();
  jw.key_value("title", spec.info.title);
  jw.key_optional("summary", spec.info.summary);
  jw.key_optional("description", spec.info.description);
  jw.key_value("version", spec.info.version);
  emit_extensions(jw, spec.info.extensions);
  jw.end_object();

  jw.key("sourceDescriptions");
  jw.start_array();
  for (const auto &sd : spec.sourceDescriptions) {
    jw.start_object();
    jw.key_value("name", sd.name);
    jw.key_value("url", sd.url);
    if (sd.type.has_value()) {
      jw.key_value("type", std::string(to_string(*sd.type)));
    }
    emit_extensions(jw, sd.extensions);
    jw.end_object();
  }
  jw.end_array();

  jw.key("workflows");
  jw.start_array();
  for (const auto &wf : spec.workflows) {
    emit_workflow(jw, wf);
  }
  jw.end_array();

  if (spec.components.has_value()) {
    jw.key("components");
    jw.start_object();
    if (spec.components->inputs.has_value()) {
      jw.key("inputs");
      jw.start_object();
      for (const auto &[k, s] : *spec.components->inputs) {
        jw.key(k);
        emit_input_schema(jw, s);
      }
      jw.end_object();
    }
    if (spec.components->parameters.has_value()) {
      jw.key("parameters");
      jw.start_object();
      for (const auto &[k, p] : *spec.components->parameters) {
        jw.key(k);
        emit_parameter(jw, p);
      }
      jw.end_object();
    }
    if (spec.components->successActions.has_value()) {
      jw.key("successActions");
      jw.start_object();
      for (const auto &[k, sa] : *spec.components->successActions) {
        jw.key(k);
        emit_success_action(jw, sa);
      }
      jw.end_object();
    }
    if (spec.components->failureActions.has_value()) {
      jw.key("failureActions");
      jw.start_object();
      for (const auto &[k, fa] : *spec.components->failureActions) {
        jw.key(k);
        emit_failure_action(jw, fa);
      }
      jw.end_object();
    }
    emit_extensions(jw, spec.components->extensions);
    jw.end_object();
  }

  emit_extensions(jw, spec.extensions);
  jw.end_object();
  return jw.str();
}

} // namespace cdd_cpp::arazzo
