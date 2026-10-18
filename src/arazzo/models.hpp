/// @file models.hpp
/// @brief Strongly typed Arazzo Specification 1.1.0 data models and
/// enumerations.

#pragma once
#include "../openapi/models.hpp"
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

/// @namespace cdd_cpp::arazzo
/// @brief Arazzo Specification 1.1.0 data models, runtime expression
/// evaluators, parsers, and emitters.
namespace cdd_cpp::arazzo {

/// @brief Map of specification extensions (`x-*`) to strongly typed JsonValue.
using ExtensionsMap = std::map<std::string, openapi::JsonValue>;

/// @brief Type of a Source Description (`"openapi"` or `"arazzo"`).
enum class SourceDescriptionType {
  OpenApi, ///< OpenAPI specification document ("openapi").
  Arazzo   ///< Arazzo specification document ("arazzo").
};

/// @brief Convert a SourceDescriptionType to its Arazzo string representation.
/// @param type The SourceDescriptionType value.
/// @return String view (`"openapi"` or `"arazzo"`).
constexpr std::string_view to_string(SourceDescriptionType type) noexcept {
  switch (type) {
  case SourceDescriptionType::OpenApi:
    return "openapi";
  case SourceDescriptionType::Arazzo:
    return "arazzo";
  }
  return "openapi";
}

/// @brief Parse a SourceDescriptionType from its string representation.
/// @param str The raw string view to parse.
/// @return Optional SourceDescriptionType, or std::nullopt if unrecognized.
constexpr std::optional<SourceDescriptionType>
source_description_type_from_string(std::string_view str) noexcept {
  if (str == "openapi")
    return SourceDescriptionType::OpenApi;
  if (str == "arazzo")
    return SourceDescriptionType::Arazzo;
  return std::nullopt;
}

/// @brief Location of an Arazzo operation parameter (`"path"`, `"query"`,
/// `"header"`, `"cookie"`).
enum class ParameterIn {
  Path,   ///< Parameter passed in URL path ("path").
  Query,  ///< Parameter passed in URL query string ("query").
  Header, ///< Parameter passed in HTTP header ("header").
  Cookie  ///< Parameter passed in HTTP cookie ("cookie").
};

/// @brief Convert a ParameterIn enumerator to its Arazzo string representation.
/// @param loc The ParameterIn value.
/// @return String view (`"path"`, `"query"`, `"header"`, or `"cookie"`).
constexpr std::string_view to_string(ParameterIn loc) noexcept {
  switch (loc) {
  case ParameterIn::Path:
    return "path";
  case ParameterIn::Query:
    return "query";
  case ParameterIn::Header:
    return "header";
  case ParameterIn::Cookie:
    return "cookie";
  }
  return "query";
}

/// @brief Parse a ParameterIn enumerator from its string representation.
/// @param str The raw string view to parse.
/// @return Optional ParameterIn, or std::nullopt if unrecognized.
constexpr std::optional<ParameterIn>
parameter_in_from_string(std::string_view str) noexcept {
  if (str == "path")
    return ParameterIn::Path;
  if (str == "query")
    return ParameterIn::Query;
  if (str == "header")
    return ParameterIn::Header;
  if (str == "cookie")
    return ParameterIn::Cookie;
  return std::nullopt;
}

/// @brief Evaluation type for a Criterion (`"simple"`, `"regex"`, `"jsonpath"`,
/// `"xpath"`).
enum class CriterionType {
  Simple,   ///< Simple boolean/comparison condition ("simple").
  Regex,    ///< Regular expression condition ("regex").
  JsonPath, ///< JSONPath query condition ("jsonpath").
  XPath     ///< XPath query condition ("xpath").
};

/// @brief Convert a CriterionType to its Arazzo string representation.
/// @param type The CriterionType value.
/// @return String view (`"simple"`, `"regex"`, `"jsonpath"`, or `"xpath"`).
constexpr std::string_view to_string(CriterionType type) noexcept {
  switch (type) {
  case CriterionType::Simple:
    return "simple";
  case CriterionType::Regex:
    return "regex";
  case CriterionType::JsonPath:
    return "jsonpath";
  case CriterionType::XPath:
    return "xpath";
  }
  return "simple";
}

/// @brief Parse a CriterionType from its string representation.
/// @param str The raw string view to parse.
/// @return Optional CriterionType, or std::nullopt if unrecognized.
constexpr std::optional<CriterionType>
criterion_type_from_string(std::string_view str) noexcept {
  if (str == "simple")
    return CriterionType::Simple;
  if (str == "regex")
    return CriterionType::Regex;
  if (str == "jsonpath")
    return CriterionType::JsonPath;
  if (str == "xpath")
    return CriterionType::XPath;
  return std::nullopt;
}

/// @brief Specification version for a CriterionExpressionType (`jsonpath` or
/// `xpath`).
enum class CriterionExpressionTypeVersion {
  DraftGoessnerDispatchJsonpath00, ///< Goessner JSONPath draft
                                   ///< ("draft-goessner-dispatch-jsonpath-00").
  Rfc9535, ///< RFC 9535 IETF JSONPath standard ("rfc9535").
  XPath30, ///< XPath 3.0 ("xpath-30").
  XPath20, ///< XPath 2.0 ("xpath-20").
  XPath10  ///< XPath 1.0 ("xpath-10").
};

/// @brief Convert a CriterionExpressionTypeVersion to its Arazzo string
/// representation.
/// @param ver The CriterionExpressionTypeVersion value.
/// @return String view representing the version identifier.
constexpr std::string_view
to_string(CriterionExpressionTypeVersion ver) noexcept {
  switch (ver) {
  case CriterionExpressionTypeVersion::DraftGoessnerDispatchJsonpath00:
    return "draft-goessner-dispatch-jsonpath-00";
  case CriterionExpressionTypeVersion::Rfc9535:
    return "rfc9535";
  case CriterionExpressionTypeVersion::XPath30:
    return "xpath-30";
  case CriterionExpressionTypeVersion::XPath20:
    return "xpath-20";
  case CriterionExpressionTypeVersion::XPath10:
    return "xpath-10";
  }
  return "rfc9535";
}

/// @brief Parse a CriterionExpressionTypeVersion from its string
/// representation.
/// @param str The raw string view to parse.
/// @return Optional CriterionExpressionTypeVersion, or std::nullopt if
/// unrecognized.
constexpr std::optional<CriterionExpressionTypeVersion>
criterion_version_from_string(std::string_view str) noexcept {
  if (str == "draft-goessner-dispatch-jsonpath-00")
    return CriterionExpressionTypeVersion::DraftGoessnerDispatchJsonpath00;
  if (str == "rfc9535")
    return CriterionExpressionTypeVersion::Rfc9535;
  if (str == "xpath-30")
    return CriterionExpressionTypeVersion::XPath30;
  if (str == "xpath-20")
    return CriterionExpressionTypeVersion::XPath20;
  if (str == "xpath-10")
    return CriterionExpressionTypeVersion::XPath10;
  return std::nullopt;
}

/// @brief Action type for a SuccessAction (`"end"` or `"goto"`).
enum class SuccessActionType {
  End, ///< Workflow ends immediately upon step success ("end").
  Goto ///< Control transfers to another stepId or workflowId ("goto").
};

/// @brief Convert a SuccessActionType to its Arazzo string representation.
/// @param type The SuccessActionType value.
/// @return String view (`"end"` or `"goto"`).
constexpr std::string_view to_string(SuccessActionType type) noexcept {
  switch (type) {
  case SuccessActionType::End:
    return "end";
  case SuccessActionType::Goto:
    return "goto";
  }
  return "end";
}

/// @brief Parse a SuccessActionType from its string representation.
/// @param str The raw string view to parse.
/// @return Optional SuccessActionType, or std::nullopt if unrecognized.
constexpr std::optional<SuccessActionType>
success_action_type_from_string(std::string_view str) noexcept {
  if (str == "end")
    return SuccessActionType::End;
  if (str == "goto")
    return SuccessActionType::Goto;
  return std::nullopt;
}

/// @brief Action type for a FailureAction (`"end"`, `"retry"`, or `"goto"`).
enum class FailureActionType {
  End,   ///< Workflow terminates in failure state ("end").
  Retry, ///< Step or target step/workflow is retried ("retry").
  Goto   ///< Control transfers to recovery stepId or workflowId ("goto").
};

/// @brief Convert a FailureActionType to its Arazzo string representation.
/// @param type The FailureActionType value.
/// @return String view (`"end"`, `"retry"`, or `"goto"`).
constexpr std::string_view to_string(FailureActionType type) noexcept {
  switch (type) {
  case FailureActionType::End:
    return "end";
  case FailureActionType::Retry:
    return "retry";
  case FailureActionType::Goto:
    return "goto";
  }
  return "end";
}

/// @brief Parse a FailureActionType from its string representation.
/// @param str The raw string view to parse.
/// @return Optional FailureActionType, or std::nullopt if unrecognized.
constexpr std::optional<FailureActionType>
failure_action_type_from_string(std::string_view str) noexcept {
  if (str == "end")
    return FailureActionType::End;
  if (str == "retry")
    return FailureActionType::Retry;
  if (str == "goto")
    return FailureActionType::Goto;
  return std::nullopt;
}

/// @brief Metadata about the Arazzo description document (`Info Object`).
struct Info {
  std::string title =
      {}; ///< REQUIRED human-readable title of the Arazzo document.
  std::optional<std::string> summary =
      std::nullopt; ///< Optional short summary of the Arazzo document.
  std::optional<std::string> description =
      std::nullopt; ///< Optional description of the purpose of the workflows.
  std::string version = {}; ///< REQUIRED version string of the Arazzo document.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other Info object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const Info &other) const noexcept = default;
};

/// @brief Reference to a source OpenAPI or Arazzo document (`Source Description
/// Object`).
struct SourceDescription {
  std::string name = {}; ///< REQUIRED unique name matching ^[A-Za-z0-9_\-]+$.
  std::string url = {};  ///< REQUIRED URI or relative reference to the
                         ///< OpenAPI/Arazzo source document.
  std::optional<SourceDescriptionType> type =
      std::nullopt; ///< Optional source description type ("openapi" or
                    ///< "arazzo").
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other SourceDescription object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const SourceDescription &other) const noexcept = default;
};

/// @brief Reference to a reusable component (`Reusable Object`).
struct ReusableObject {
  std::string reference =
      {}; ///< REQUIRED runtime expression referencing
          ///< $components.(parameters|successActions|failureActions).<name>.
  std::optional<openapi::JsonValue> value =
      std::nullopt; ///< Optional overridden value when referencing a reusable
                    ///< parameter.

  /// @brief Equality comparison operator.
  /// @param other The other ReusableObject to compare against.
  /// @return True if all fields are equal.
  bool operator==(const ReusableObject &other) const noexcept = default;
};

/// @brief A parameter passed to an operation or sub-workflow (`Parameter
/// Object`).
struct Parameter {
  std::string name = {}; ///< REQUIRED parameter name.
  std::optional<ParameterIn> in =
      std::nullopt; ///< Parameter location (path, query, header, cookie);
                    ///< nullopt when targeting a sub-workflow.
  openapi::JsonValue value =
      nullptr; ///< REQUIRED literal value or runtime expression string.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other Parameter object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const Parameter &other) const noexcept = default;
};

/// @brief Strongly typed union of inline Parameter or ReusableObject reference.
using ParameterOrReusable = std::variant<Parameter, ReusableObject>;

/// @brief Dynamic replacement applied to a location within a request payload
/// (`Payload Replacement Object`).
struct PayloadReplacement {
  std::string target = {}; ///< REQUIRED JSON Pointer or XPath expression
                           ///< targeting a location inside the payload.
  openapi::JsonValue value = nullptr; ///< REQUIRED literal value or runtime
                                      ///< expression injected at target.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other PayloadReplacement object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const PayloadReplacement &other) const noexcept = default;
};

/// @brief Request body definition for a step (`Request Body Object`).
struct RequestBody {
  std::optional<std::string> contentType =
      std::nullopt; ///< Optional Content-Type media type for the request
                    ///< payload.
  std::optional<openapi::JsonValue> payload =
      std::nullopt; ///< Optional literal JSON value, string template, or
                    ///< runtime expression payload.
  std::optional<std::vector<PayloadReplacement>> replacements =
      std::nullopt; ///< Optional ordered list of dynamic payload replacements.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other RequestBody object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const RequestBody &other) const noexcept = default;
};

/// @brief Versioned expression type object for a Criterion (`Criterion
/// Expression Type Object`).
struct CriterionExpressionType {
  CriterionType type =
      CriterionType::JsonPath; ///< REQUIRED expression evaluation type
                               ///< (jsonpath or xpath).
  CriterionExpressionTypeVersion version = CriterionExpressionTypeVersion::
      Rfc9535; ///< REQUIRED version of the JSONPath/XPath specification.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other CriterionExpressionType object to compare against.
  /// @return True if all fields are equal.
  bool
  operator==(const CriterionExpressionType &other) const noexcept = default;
};

/// @brief Strongly typed union of simple CriterionType or versioned
/// CriterionExpressionType.
using CriterionTypeVariant =
    std::variant<CriterionType, CriterionExpressionType>;

/// @brief Assertion condition evaluated on step execution (`Criterion Object`).
struct Criterion {
  std::optional<std::string> context =
      std::nullopt; ///< Optional runtime expression providing evaluation
                    ///< context (e.g., $statusCode, $response.body).
  std::string condition = {}; ///< REQUIRED boolean condition, regex pattern,
                              ///< JSONPath, or XPath expression.
  std::optional<CriterionTypeVariant> type =
      std::nullopt; ///< Optional criterion evaluation type (defaults to Simple
                    ///< when nullopt).
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other Criterion object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const Criterion &other) const noexcept = default;
};

/// @brief Action taken when a step succeeds (`Success Action Object`).
struct SuccessAction {
  std::string name =
      {}; ///< REQUIRED case-sensitive name of the success action.
  SuccessActionType type =
      SuccessActionType::End; ///< REQUIRED action type ("end" or "goto").
  std::optional<std::string> workflowId =
      std::nullopt; ///< Optional target workflowId when type is Goto.
  std::optional<std::string> stepId =
      std::nullopt; ///< Optional target stepId within current workflow when
                    ///< type is Goto.
  std::optional<std::vector<Criterion>> criteria =
      std::nullopt; ///< Optional additional criteria guarding this action.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other SuccessAction object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const SuccessAction &other) const noexcept = default;
};

/// @brief Strongly typed union of inline SuccessAction or ReusableObject
/// reference.
using SuccessActionOrReusable = std::variant<SuccessAction, ReusableObject>;

/// @brief Action taken when a step fails (`Failure Action Object`).
struct FailureAction {
  std::string name =
      {}; ///< REQUIRED case-sensitive name of the failure action.
  FailureActionType type =
      FailureActionType::End; ///< REQUIRED action type ("end", "retry", or
                              ///< "goto").
  std::optional<std::string> workflowId =
      std::nullopt; ///< Optional target workflowId when type is Goto or Retry.
  std::optional<std::string> stepId =
      std::nullopt; ///< Optional target stepId when type is Goto or Retry.
  std::optional<double> retryAfter =
      std::nullopt; ///< Optional non-negative delay in seconds before retrying.
  std::optional<int64_t> retryLimit =
      std::nullopt; ///< Optional maximum number of retry attempts allowed.
  std::optional<std::vector<Criterion>> criteria =
      std::nullopt; ///< Optional criteria guarding when this failure action
                    ///< triggers.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other FailureAction object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const FailureAction &other) const noexcept = default;
};

/// @brief Strongly typed union of inline FailureAction or ReusableObject
/// reference.
using FailureActionOrReusable = std::variant<FailureAction, ReusableObject>;

/// @brief A single workflow execution step (`Step Object`).
struct Step {
  std::string stepId = {}; ///< REQUIRED unique step identifier within the
                           ///< workflow (^[A-Za-z0-9_\-]+$).
  std::optional<std::string> description =
      std::nullopt; ///< Optional description of the step.
  std::optional<std::string> operationId =
      std::nullopt; ///< Target OpenAPI operationId or
                    ///< $sourceDescriptions.<name>.<operationId> (mutually
                    ///< exclusive with operationPath and workflowId).
  std::optional<std::string> operationPath =
      std::nullopt; ///< Target OpenAPI operation JSON Pointer (mutually
                    ///< exclusive with operationId and workflowId).
  std::optional<std::string> workflowId =
      std::nullopt; ///< Target sub-workflow identifier (mutually exclusive with
                    ///< operationId and operationPath).
  std::optional<std::vector<ParameterOrReusable>> parameters =
      std::nullopt; ///< Optional parameters or ReusableObject references passed
                    ///< to the operation/workflow.
  std::optional<RequestBody> requestBody =
      std::nullopt; ///< Optional request body payload and dynamic replacements.
  std::optional<std::vector<Criterion>> successCriteria =
      std::nullopt; ///< Optional assertions that must hold for the step to
                    ///< succeed.
  std::optional<std::vector<SuccessActionOrReusable>> onSuccess =
      std::nullopt; ///< Optional actions executed when successCriteria pass.
  std::optional<std::vector<FailureActionOrReusable>> onFailure =
      std::nullopt; ///< Optional actions executed when successCriteria or
                    ///< transport fail.
  std::optional<std::map<std::string, std::string>> outputs =
      std::nullopt; ///< Optional map of step output name to runtime expression.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other Step object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const Step &other) const noexcept = default;
};

/// @brief Deep equality comparison for `openapi::Schema` (comparing
/// `properties` and `items` by value rather than `shared_ptr` address).
/// @param a First Schema to compare.
/// @param b Second Schema to compare.
/// @return True if both schemas have identical values.
inline bool schemas_equal(const openapi::Schema &a,
                          const openapi::Schema &b) noexcept {
  if (a.type != b.type || a.title != b.title ||
      a.description != b.description || a.format != b.format ||
      a.default_value != b.default_value || a.required != b.required) {
    return false;
  }
  if ((a.properties == nullptr) != (b.properties == nullptr)) {
    return false;
  }
  if (a.properties != nullptr) {
    if (a.properties->size() != b.properties->size()) {
      return false;
    }
    for (const auto &[k, va] : *a.properties) {
      auto it = b.properties->find(k);
      if (it == b.properties->end() || !schemas_equal(va, it->second)) {
        return false;
      }
    }
  }
  if ((a.items == nullptr) != (b.items == nullptr)) {
    return false;
  }
  if (a.items != nullptr && !schemas_equal(*a.items, *b.items)) {
    return false;
  }
  return true;
}

/// @brief An orchestrated multi-step API workflow (`Workflow Object`).
struct Workflow {
  std::string workflowId =
      {}; ///< REQUIRED unique workflow identifier (^[A-Za-z0-9_\-]+$).
  std::optional<std::string> summary =
      std::nullopt; ///< Optional short summary of what the workflow performs.
  std::optional<std::string> description =
      std::nullopt; ///< Optional verbose description of the workflow behavior.
  std::optional<openapi::Schema> inputs =
      std::nullopt; ///< Optional JSON Schema 2020-12 object describing workflow
                    ///< inputs.
  std::optional<std::vector<std::string>> dependsOn =
      std::nullopt;             ///< Optional list of prerequisite workflowIds.
  std::vector<Step> steps = {}; ///< REQUIRED ordered list of steps comprising
                                ///< the workflow (at least 1).
  std::optional<std::vector<SuccessActionOrReusable>> successActions =
      std::nullopt; ///< Optional workflow-level default success actions.
  std::optional<std::vector<FailureActionOrReusable>> failureActions =
      std::nullopt; ///< Optional workflow-level default failure actions.
  std::optional<std::map<std::string, std::string>> outputs =
      std::nullopt; ///< Optional map of workflow output name to runtime
                    ///< expression.
  std::optional<std::vector<ParameterOrReusable>> parameters =
      std::nullopt; ///< Optional workflow-level default parameters inherited by
                    ///< steps.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other Workflow object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const Workflow &other) const noexcept {
    if (inputs.has_value() != other.inputs.has_value()) {
      return false;
    }
    if (inputs.has_value() && !schemas_equal(*inputs, *other.inputs)) {
      return false;
    }
    return workflowId == other.workflowId && summary == other.summary &&
           description == other.description && dependsOn == other.dependsOn &&
           steps == other.steps && successActions == other.successActions &&
           failureActions == other.failureActions && outputs == other.outputs &&
           parameters == other.parameters && extensions == other.extensions;
  }
};

/// @brief Reusable Arazzo components (`Components Object`).
struct Components {
  std::optional<std::map<std::string, openapi::Schema>> inputs =
      std::nullopt; ///< Optional map of reusable JSON Schema input definitions.
  std::optional<std::map<std::string, Parameter>> parameters =
      std::nullopt; ///< Optional map of reusable Parameter definitions.
  std::optional<std::map<std::string, SuccessAction>> successActions =
      std::nullopt; ///< Optional map of reusable SuccessAction definitions.
  std::optional<std::map<std::string, FailureAction>> failureActions =
      std::nullopt; ///< Optional map of reusable FailureAction definitions.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other Components object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const Components &other) const noexcept {
    if (inputs.has_value() != other.inputs.has_value()) {
      return false;
    }
    if (inputs.has_value()) {
      if (inputs->size() != other.inputs->size()) {
        return false;
      }
      for (const auto &[k, va] : *inputs) {
        auto it = other.inputs->find(k);
        if (it == other.inputs->end() || !schemas_equal(va, it->second)) {
          return false;
        }
      }
    }
    return parameters == other.parameters &&
           successActions == other.successActions &&
           failureActions == other.failureActions &&
           extensions == other.extensions;
  }
};

/// @brief Root Arazzo Specification 1.1.0 Document (`Arazzo Object`).
struct Arazzo {
  std::string arazzo = "1.1.0"; ///< REQUIRED Arazzo Specification version
                                ///< string ("1.1.0", "1.0.1", "1.0.0").
  Info info = {}; ///< REQUIRED metadata about the Arazzo document.
  std::vector<SourceDescription> sourceDescriptions =
      {}; ///< REQUIRED list of source descriptions (at least 1).
  std::vector<Workflow> workflows =
      {}; ///< REQUIRED list of workflows (at least 1).
  std::optional<Components> components =
      std::nullopt; ///< Optional reusable components object.
  std::optional<ExtensionsMap> extensions =
      std::nullopt; ///< Optional top-level x-* specification extensions.

  /// @brief Equality comparison operator.
  /// @param other The other Arazzo object to compare against.
  /// @return True if all fields are equal.
  bool operator==(const Arazzo &other) const noexcept = default;
};

} // namespace cdd_cpp::arazzo
