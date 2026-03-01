# OpenAPI 3.2.0 Compliance Report

`cdd-cpp` has been updated to parse and emit structural components of the OpenAPI 3.2.0 specification. 

## Fully Supported 3.2.0 Features:
- The `$self` field at the root `OpenAPI` object.
- The `webhooks` mapping at the root `OpenAPI` object.
- The `jsonSchemaDialect` keyword at the root level to support Draft 2020-12 by default.
- Structural `Schema` updates matching JSON Schema Draft 2020-12, including:
  - `$id`, `$anchor`, `$dynamicAnchor`, `$dynamicRef`, `$defs`
  - `prefixItems`, `unevaluatedItems`, `unevaluatedProperties`
  - `dependentRequired`
- The `QUERY` HTTP method in `PathItem` objects, alongside `GET`, `POST`, `PUT`, `PATCH`, `DELETE`, `OPTIONS`, `HEAD`, `TRACE`.

## Pending/Partial Support:
- Deep recursive resolution of `$dynamicRef` across different external documents.
- Full runtime validation of JSON Schema Draft 2020-12 constructs (this library is primarily a structural parser/emitter, not a runtime schema validator).
- Callbacks implementation may need further refinement for edge cases in 3.2.0.

The parser and emitters correctly serialize and deserialize these fields to/from JSON. Further repetitions of this prompt can be used to achieve 100% compliance on the pending edge cases.
