
cdd-LANGUAGE
============

[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CI/CD](https://github.com/offscale/cdd-cpp/workflows/CI/badge.svg)](https://github.com/offscale/cdd-cpp/actions)
![Test Coverage](https://img.shields.io/badge/coverage-100.00%25-brightgreen.svg)
![Doc Coverage](https://img.shields.io/badge/docs-100.00%25-brightgreen.svg)

OpenAPI ↔ C++. This is one compiler in a suite, all focussed on the same task: Compiler Driven Development (CDD).

Each compiler is written in its target language, is whitespace and comment sensitive, and has both an SDK and CLI.

The CLI—at a minimum—has:
- `cdd-LANGUAGE --help`
- `cdd-LANGUAGE --version`
- `cdd-LANGUAGE from_openapi -i spec.json`
- `cdd-LANGUAGE to_openapi -f path/to/code`
- `cdd-LANGUAGE to_docs_json --no-imports --no-wrapping -i spec.json`

The goal of this project is to enable rapid application development without tradeoffs. Tradeoffs of Protocol Buffers / Thrift etc. are an untouchable "generated" directory and package, compile-time and/or runtime overhead. Tradeoffs of Java or JavaScript for everything are: overhead in hardware access, offline mode, ML inefficiency, and more. And neither of these alterantive approaches are truly integrated into your target system, test frameworks, and bigger abstractions you build in your app. Tradeoffs in CDD are code duplication (but CDD handles the synchronisation for you).

## 🚀 Capabilities

The `cdd-cpp` compiler leverages a unified architecture to support various facets of API and code lifecycle management.

* **Compilation**:
  * **OpenAPI → `C++`**: Generate idiomatic native models, network routes, client SDKs, database schemas, and boilerplate directly from OpenAPI (`.json` / `.yaml`) specifications.
  * **`C++` → OpenAPI**: Statically parse existing `C++` source code and emit compliant OpenAPI specifications.
* **AST-Driven & Safe**: Employs static analysis (Abstract Syntax Trees) instead of unsafe dynamic execution or reflection, allowing it to safely parse and emit code even for incomplete or un-compilable project states.
* **Seamless Sync**: Keep your docs, tests, database, clients, and routing in perfect harmony. Update your code, and generate the docs; or update the docs, and generate the code.

## 📦 Installation

<!-- INSTRUCTION TO LLM: Insert specific installation instructions, package managers, and prerequisites here based on the `Requires a C++17 compliant compiler, CMake 3.14+, and Doxygen. Build from source:
```bash
make install_base
make build
# Binary is located in custom_bin/cdd-cpp
```` context. -->

## 🛠 Usage

### Command Line Interface

```bash
# Generate C++ Client from OpenAPI spec
./custom_bin/cdd-cpp from_openapi -i spec.json

# Parse C++ project and output OpenAPI spec
./custom_bin/cdd-cpp to_openapi -f src/
```

### Programmatic SDK / Library

```cpp
#include "openapi/parse.hpp"
#include "classes/emit_client.hpp"

// Read OpenAPI spec and emit a C++ Client
int main() {
    auto spec = cdd_cpp::openapi::parse(R"({"openapi":"3.2.0"})");
    std::string code = cdd_cpp::classes::emit_client(spec);
    return 0;
}
```

## Design choices

We opted for a custom lightweight AST parser tailored for C++ structures typical in APIs rather than linking heavy compilers like Clang/LLVM. This dramatically speeds up cross-platform builds and enables simpler compilation to WebAssembly (WASM).

## 🏗 Supported Conversions for C++

*(The boxes below reflect the features supported by this specific `cdd-cpp` implementation)*

| Concept | Parse (From) | Emit (To) |
|---------|--------------|-----------|
| OpenAPI (JSON/YAML) | ✅ | ✅ |
| `C++` Models / Structs / Types | ✅ | ✅ |
| `C++` Server Routes / Endpoints | ✅ | ✅ |
| `C++` API Clients / SDKs | ✅ | ✅ |
| `C++` ORM / DB Schemas | [ ] | [ ] |
| `C++` CLI Argument Parsers | [ ] | [ ] |
| `C++` Docstrings / Comments | ✅ | ✅ |

<!-- INSTRUCTION TO LLM: Check the boxes above (`✅`) based on the `{SUPPORTED_CONVERSIONS}` context provided. -->

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
