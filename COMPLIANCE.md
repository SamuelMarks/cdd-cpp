# OpenAPI Compliance

This document tracks `cdd-cpp`s compliance with OpenAPI specification requirements.

Currently, we are targeting [OpenAPI 3.2.0](https://raw.githubusercontent.com/OAI/OpenAPI-Specification/refs/heads/main/versions/3.2.0.md).

## Compliance Status

- [x] Basic Document Structure (`openapi`, `info`, `servers`, `paths`, `components`, `security`, `tags`, `externalDocs`, `webhooks`)
- [x] `$self` Link implementation
- [x] Paths and Operations (including `additionalOperations`)
- [x] Media Type Object updates (`itemSchema`, `prefixEncoding`, `itemEncoding`)
- [x] Components (Schemas, Parameters, Responses, Headers, MediaTypes)
- [x] Schema Draft 2020-12 Integration (most keywords supported in internal AST, including `$schema`, `$id`, `dynamicRef`, etc)
- [x] Discriminator `defaultMapping` support
- [x] Tag Object extensions (`parent` and `kind`)
- [ ] Runtime Expressions
- [ ] Mutually Exclusive Security Schemes validation

As the parser and emitter logic expands, this list will be updated until 100% compliance is reached.
