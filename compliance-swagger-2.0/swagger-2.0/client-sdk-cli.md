# Swagger 2.0 Conformance Table: Client SDK CLI (CLI Tooling & Tests)

This table tracks the completeness of language integration with Swagger 2.0 for generating Command-Line Interfaces (CLIs) wrapper tools, and vice-versa.

### Legend & Tracking Guide
*   **To**: Language -> OpenAPI (Generating the Swagger document from declarative CLI structures)
*   **From**: OpenAPI -> Language (Generating CLI routing, flag parsing, and formatting from Swagger)
*   **Presence `[To, From]`**: The object is successfully parsed, validated, utilized, or generated.
*   **Absence `[To, From]`**: The object is currently unsupported, dropped, or falls back to generic/`any` types.
*   **Skipped `[To, From]`**: Intentionally ignored because it is irrelevant or unsupported by the architecture.
*   **Checkboxes**: Mark `[x]` as conformance is achieved.

| Swagger 2.0 Object / Feature | Presence `[To, From]` | Absence `[To, From]` | Skipped `[To, From]` | Notes / Implementation Strategy |
| :--- | :---: | :---: | :---: | :--- |
| **Swagger Object (Root)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`swagger`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`info`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`host`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`basePath`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`schemes`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`consumes`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`produces`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`paths`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`definitions`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`parameters`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`responses`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`securityDefinitions`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`security`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`tags`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`externalDocs`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Swagger Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object (`title`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object (`termsOfService`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object (`contact`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object (`license`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object (`version`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Info Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Contact Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Contact Object (`name`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Contact Object (`url`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Contact Object (`email`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Contact Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **License Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **License Object (`name`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **License Object (`url`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **License Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Paths Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Paths Object (`/{path}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Paths Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`$ref`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`get`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`put`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`post`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`delete`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`options`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`head`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`patch`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`parameters`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Path Item Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`tags`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`summary`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`externalDocs`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`operationId`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`consumes`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`produces`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`parameters`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`responses`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`schemes`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`deprecated`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`security`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Operation Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **External Documentation Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **External Documentation Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **External Documentation Object (`url`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **External Documentation Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`name`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`in`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`required`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`schema`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`type`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`format`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`allowEmptyValue`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`items`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`collectionFormat`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`default`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`maximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`exclusiveMaximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`minimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`exclusiveMinimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`maxLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`minLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`pattern`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`maxItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`minItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`uniqueItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`enum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`multipleOf`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameter Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`type`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`format`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`items`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`collectionFormat`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`default`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`maximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`exclusiveMaximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`minimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`exclusiveMinimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`maxLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`minLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`pattern`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`maxItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`minItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`uniqueItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`enum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`multipleOf`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Items Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Responses Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Responses Object (`default`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Responses Object (`{HTTP Status Code}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Responses Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Response Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Response Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Response Object (`schema`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Response Object (`headers`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Response Object (`examples`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Response Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Headers Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Headers Object (`{name}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Example Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Example Object (`{mime type}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`type`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`format`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`items`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`collectionFormat`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`default`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`maximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`exclusiveMaximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`minimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`exclusiveMinimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`maxLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`minLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`pattern`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`maxItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`minItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`uniqueItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`enum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`multipleOf`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Header Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Tag Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Tag Object (`name`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Tag Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Tag Object (`externalDocs`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Tag Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Reference Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Reference Object (`$ref`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`$ref`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`format`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`title`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`default`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`multipleOf`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`maximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`exclusiveMaximum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`minimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`exclusiveMinimum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`maxLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`minLength`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`pattern`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`maxItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`minItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`uniqueItems`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`maxProperties`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`minProperties`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`required`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`enum`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`type`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`items`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`allOf`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`properties`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`additionalProperties`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`discriminator`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`readOnly`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`xml`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`externalDocs`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`example`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Schema Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **XML Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **XML Object (`name`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **XML Object (`namespace`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **XML Object (`prefix`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **XML Object (`attribute`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **XML Object (`wrapped`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **XML Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Definitions Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Definitions Object (`{name}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameters Definitions Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Parameters Definitions Object (`{name}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Responses Definitions Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Responses Definitions Object (`{name}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Definitions Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Definitions Object (`{name}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`type`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`description`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`name`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`in`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`flow`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`authorizationUrl`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`tokenUrl`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`scopes`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Scheme Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Scopes Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Scopes Object (`{name}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Scopes Object (`^x-`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Requirement Object** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
| **Security Requirement Object (`{name}`)** | `[-]` , `[-]` | `[-]` , `[-]` | `[x]` , `[x]` | Upgraded to OAS 3.2.0 via upgrader.cpp |
