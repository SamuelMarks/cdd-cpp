# OpenAPI, Swagger, Arazzo & MCP Spec Compliance

Supported specifications:
- **[OpenAPI 3.2.0](https://spec.openapis.org/oas/v3.2.0.html)** (plus automatic upgrading from **Swagger 1.2**, **Swagger 2.0**, **OpenAPI 3.0.x**, and **OpenAPI 3.1.x** via `cdd_cpp::openapi::upgraders::upgrade_to_latest`)
- **[Arazzo 1.1.0](https://spec.openapis.org/arazzo/v1.1.0.html)** (full bidirectional parsing, emission, ABNF runtime expressions, and workflow orchestration)
- **[Model Context Protocol (MCP) 2024-11-05](https://spec.modelcontextprotocol.io/specification/2024-11-05/)** (full schema coverage and STDIO server)
- **[Google Discovery Document Format](https://developers.google.com/discovery/v1/reference/apis)** (ingestion into OpenAPI 3.2.0 ASTs)

## OpenAPI 3.2.0 & JSON Schema 2020-12 Support Matrix

| Specification Concept | Parse (JSON -> AST) | Emit (AST -> JSON) | Code Gen (AST -> C++) | Code Parse (C++ -> AST) |
| :--- | :---: | :---: | :---: | :---: |
| `OpenAPI` Root Object (`3.2.0`, `$self`, `jsonSchemaDialect`) | ✅ | ✅ | ✅ | ✅ |
| `Info`, `Contact`, `License` (`identifier`) | ✅ | ✅ | ✅ | ✅ |
| `Server` & `ServerVariable` | ✅ | ✅ | ✅ | ✅ |
| `Paths`, `PathItem` (`query`, `additionalOperations`), `webhooks` | ✅ | ✅ | ✅ | ✅ |
| `Operation`, `ExternalDocumentation`, `Callback` | ✅ | ✅ | ✅ | ✅ |
| `Parameter` (`path`, `query`, `header`, `cookie`, `content`, `examples`) | ✅ | ✅ | ✅ | ✅ |
| `RequestBody`, `MediaType` (`itemSchema`, `itemEncoding`), `Encoding` | ✅ | ✅ | ✅ | ✅ |
| `Responses`, `Response`, `Header`, `Link`, `Example` (`dataValue`, `serializedValue`) | ✅ | ✅ | ✅ | ✅ |
| `Components` (all 10 component maps) | ✅ | ✅ | ✅ | ✅ |
| `Schema` (JSON Schema Draft 2020-12, `oneOf`/`anyOf`/`allOf`/`not`, `prefixItems`, `$defs`, `discriminator`, `xml`) | ✅ | ✅ | ✅ | ✅ |
| `SecurityScheme` (`apiKey`, `http`, `mutualTLS`, `oauth2`, `openIdConnect`, `oauth2MetadataUrl`) & `OAuthFlows` (`deviceAuthorization`) | ✅ | ✅ | ✅ | ✅ |
| `Tag` (`summary`, `parent`, `kind`) | ✅ | ✅ | ✅ | ✅ |
| Specification Extensions (`x-*`) | ✅ | ✅ | ✅ | ✅ |

## Arazzo 1.1.0 Support Matrix

| Specification Concept | Parse (JSON -> AST) | Emit (AST -> JSON) | Code Gen (AST -> C++) | Code Parse (C++ -> AST) |
| :--- | :---: | :---: | :---: | :---: |
| `Arazzo` Root (`1.1.0`, `info`, `sourceDescriptions`, `workflows`, `components`) | ✅ | ✅ | ✅ | ✅ |
| `SourceDescription` (`openapi`, `arazzo`) | ✅ | ✅ | ✅ | ✅ |
| `Workflow` (`workflowId`, `inputs`, `dependsOn`, `steps`, `successActions`, `failureActions`, `outputs`, `parameters`) | ✅ | ✅ | ✅ | ✅ |
| `Step` (`stepId`, `operationId`, `operationPath`, `workflowId`, `parameters`, `requestBody`, `successCriteria`, `onSuccess`, `onFailure`, `outputs`) | ✅ | ✅ | ✅ | ✅ |
| `RequestBody` & `PayloadReplacement` (RFC 6901 JSON Pointer) | ✅ | ✅ | ✅ | ✅ |
| `Criterion` (`simple`, `regex`, `jsonpath` RFC 9535 / Goessner, `xpath` 1.0/2.0/3.0) | ✅ | ✅ | ✅ | ✅ |
| `SuccessAction` (`end`, `goto`), `FailureAction` (`end`, `retry`, `goto`, `retryAfter`, `retryLimit`) & `ReusableObject` | ✅ | ✅ | ✅ | ✅ |
| ABNF Runtime Expressions (`$url`, `$method`, `$statusCode`, `$request.*`, `$response.*`, `$inputs.*`, `$outputs.*`, `$steps.*`, `$workflows.*`, `$sourceDescriptions.*`, `$components.*`, `$self`) | ✅ | ✅ | ✅ | ✅ |
| Specification Extensions (`x-*`) | ✅ | ✅ | ✅ | ✅ |
