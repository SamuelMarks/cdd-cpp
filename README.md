cdd-cpp
============

[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI](https://github.com/SamuelMarks/cdd-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/SamuelMarks/cdd-cpp/actions/workflows/ci.yml)
[![Test Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen.svg)](#)
[![Doc Coverage](https://img.shields.io/badge/docs-100%25-brightgreen.svg)](#)

OpenAPI ↔ C++. This is one compiler in a suite, all focussed on the same task: Compiler Driven Development (CDD).

Each compiler is written in its target language, is whitespace and comment sensitive, and has both an SDK and CLI.

The CLI—at a minimum—has:
- `cdd-cpp --help`
- `cdd-cpp --version`
- `cdd-cpp from_openapi -i spec.json`
- `cdd-cpp to_openapi -f path/to/code`
- `cdd-cpp to_docs_json --no-imports --no-wrapping -i spec.json`

The goal of this project is to enable rapid application development without tradeoffs. Tradeoffs of Protocol Buffers / Thrift etc. are an untouchable "generated" directory and package, compile-time and/or runtime overhead. Tradeoffs of Java or JavaScript for everything are: overhead in hardware access, offline mode, ML inefficiency, and more. And neither of these alterantive approaches are truly integrated into your target system, test frameworks, and bigger abstractions you build in your app. Tradeoffs in CDD are code duplication (but CDD handles the synchronisation for you).

## 🚀 Capabilities

The `cdd-cpp` compiler leverages a unified architecture to support various facets of API and code lifecycle management.

* **Compilation**:
  * **OpenAPI → C++**: Generate idiomatic native models, network routes, client SDKs, database schemas, and boilerplate directly from OpenAPI (`.json` / `.yaml`) specifications.
  * **C++ → OpenAPI**: Statically parse existing C++ source code and emit compliant OpenAPI specifications.
* **Format Upgrades**: Automatically upgrades older Swagger (2.0) and OpenAPI (3.0/3.1) specifications to the latest supported OpenAPI 3.2.0 format.
* **Google Discovery Integration**: Parses Google Discovery API JSON files and maps them into standard OpenAPI specifications.
* **AST-Driven & Safe**: Employs static analysis (Abstract Syntax Trees) instead of unsafe dynamic execution or reflection, allowing it to safely parse and emit code even for incomplete or un-compilable project states.
* **Seamless Sync**: Keep your docs, tests, database, clients, and routing in perfect harmony. Update your code, and generate the docs; or update the docs, and generate the code.

## 📦 Installation

```bash
git clone https://github.com/SamuelMarks/cdd-cpp
cd cdd-cpp
cmake -S . -B build
cmake --build build
```

## 🛠 Usage

### Command Line Interface

```bash
# Generate C++ SDK from OpenAPI
cdd-cpp from_openapi to_sdk -i openapi.json -o ./MyClientSDK

# Generate OpenAPI from existing C++ models
cdd-cpp to_openapi -f src/models.cpp -o openapi.json

# Serve JSON-RPC
cdd-cpp serve_json_rpc --port 8082 --listen 0.0.0.0
```

WASM support is possible and implemented. See [WASM.md](WASM.md) for details.

## Design choices

Custom AST parsing and static analysis are used for robust, source-accurate manipulation. This ensures that accurate context and structure are preserved.

## 🏗 Supported Conversions for C++

*(The boxes below reflect the features supported by this specific `cdd-cpp` implementation)*

| Concept | Parse (From) | Emit (To) |
|---------|--------------|-----------|
| OpenAPI (JSON/YAML) | [✅] | [✅] |
| Google Discovery API | [✅] | [ ] |
| C++ Models / Structs / Types | [✅] | [✅] |
| C++ Server Routes / Endpoints | [✅] | [✅] |
| C++ API Clients / SDKs | [✅] | [✅] |
| C++ ORM / DB Schemas | [✅] | [✅] |
| C++ CLI Argument Parsers | [✅] | [✅] |
| C++ Docstrings / Comments | [✅] | [✅] |

---

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or <https://www.apache.org/licenses/LICENSE-2.0>)
- MIT license ([LICENSE-MIT](LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you, as defined in the Apache-2.0 license, shall be
dual licensed as above, without any additional terms or conditions.
