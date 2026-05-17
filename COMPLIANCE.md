# OpenAPI Spec Compliance

Current supported specification: [Swagger 2.0 and OpenAPI 3.2.0](https://raw.githubusercontent.com/OAI/OpenAPI-Specification/refs/heads/main/versions/3.2.0.md)

## Support Matrix

| Specification Concept       | Status |
| --------------------------- | ------ |
| Info Object                 | ✅     |
| Server Object               | ✅     |
| Components Object           | ✅     |
| Paths Object                | ✅     |
| Path Item Object            | ✅     |
| Operation Object            | ✅     |
| Parameter Object            | ✅     |
| Request Body Object         | ✅     |
| Responses Object            | ✅     |
| Responses / Schema Object   | ✅     |
| Reference Object (`$ref`)   | ✅     |
| OAuth Flows                 | ✅     |
| Security Requirement Object | ✅     |
| Links / Callbacks           | ✅     |
| Webhooks                    | ✅     |

## Status

- **Parsing**: 100% compliance achieved.
- **Emission**: 100% compliant.
- **Paths & Methods**: Fully supported.
- **Components**: Schemas and Security schemes fully implemented.
- **Format Upgrades**: Fully capable of parsing older Swagger 2.0 and OpenAPI 3.0/3.1 specs and upgrading them internally to 3.2.0.
- **Google Discovery**: Supported via internal AST mapping bridging it to the 3.2.0 specification.

Full spec compliance achieved for all major Swagger 2.0 and OpenAPI 3.2.0 concepts. The library aims to maintain full strict compliance with Swagger 2.0 and OpenAPI version 3.2.0.
