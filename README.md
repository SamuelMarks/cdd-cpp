cdd-cpp
=======
[![License](https://img.shields.io/badge/license-Apache--2.0%20OR%20MIT-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![interactive WASM web demo](https://img.shields.io/badge/interactive-WASM_web_demo-blue.svg)](https://offscale.io/wasm_web_demo)
[![CI](https://github.com/SamuelMarks/cdd-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/SamuelMarks/cdd-cpp/actions)
[![Test Coverage](https://img.shields.io/badge/coverage-Lines: 100.00%, Functions: 100.00%, Branches: 100.00%%25-brightgreen.svg)](#)
[![Doc Coverage](https://img.shields.io/badge/docs-100.00%25-brightgreen.svg)](#)

----

OpenAPI ↔ C++. This is one compiler in a suite, all focussed on the same task: Compiler Driven Development (CDD).

Each compiler is written in its target language, is whitespace and comment sensitive, and has both an SDK and CLI.

The core philosophy of Compiler Driven Development (CDD) is synchronization without compromise. Where traditional generators silo your API boundaries into read-only files, this compiler natively merges changes into your codebase via a robust, [whitespace and comment aware] Abstract Syntax Tree (AST) driven parser & emitter. It bridges the gap between design and implementation, allowing you to seamlessly generate SDKs from a spec or extract a spec from existing code. By keeping your APIs, SDKs, and tests in continuous, automated alignment, it drastically improves both delivery speed and software reliability.

The CLI—at a minimum—has:

- `cdd-cpp --help`
- `cdd-cpp --version`
- `cdd-cpp from_openapi to_sdk_cli -i spec.json`
- `cdd-cpp from_openapi to_sdk -i spec.json`
- `cdd-cpp from_openapi to_server -i spec.json`
- `cdd-cpp to_openapi -f path/to/code`
- `cdd-cpp to_docs_json --no-imports --no-wrapping -i spec.json`
- `cdd-cpp serve_json_rpc --port 8080 --listen 0.0.0.0`

## SDK Example

```cpp
#include "cdd_api.hpp"
#include <iostream>

int main() {
    cdd::Config config{"spec.json", "src/models"};
    cdd::generate_sdk(config);
    std::cout << "SDK generation complete.\n";
    return 0;
}
```

## Installation

```bash
cmake -B build && cmake --build build
```

## Development

You can use standard CMake commands or the included cross-platform python script to build and test:

```bash
# Standard CMake build & test
cmake -B build -S .
cmake --build build
ctest --test-dir build

# Or use the unified pre-commit checks script
python3 scripts/run_pre_commit_checks.py
```

See [PUBLISH.md](PUBLISH.md) for packaging and releasing.

## Features

The `cdd-cpp` compiler leverages a unified architecture to support various facets of API and code lifecycle management. For a deep dive into the compiler's design, see [ARCHITECTURE.md](ARCHITECTURE.md).

- **Compilation**:
    - **OpenAPI → `C++`**: Generate idiomatic native models, network routes, client SDKs, and boilerplate directly from OpenAPI (`.json` / `.yaml`) specifications.
    - **`C++` → OpenAPI**: Statically parse existing `C++` source code and emit compliant OpenAPI specifications.
- **AST-Driven & Safe**: Employs static analysis instead of unsafe dynamic execution or reflection, allowing it to safely parse and emit code even for incomplete or un-compilable project states.
- **Seamless Sync**: Keep your docs, tests, database, clients, and routing in perfect harmony. Update your code, and generate the docs; or update the docs, and generate the code.

**Uncommon Features:**

`cdd-cpp` includes specific parsing features beyond the base specification:
- **Format Upgrades:** Automatically parses older Swagger (2.0) and OpenAPI (3.0/3.1) and upgrades them to the Swagger 2.0 and OpenAPI 3.2.0 internal representation.
- **Google Discovery Integration:** Can parse Google Discovery API JSON files and map them to OpenAPI specifications natively.
- **Model Context Protocol (MCP):** Run `cdd-cpp` as an MCP server (`cdd-cpp mcp`) to dynamically interact with codebases and generate SDKs using stdio or JSON-RPC.

---

## CLI Options

```text
Usage: cdd-cpp [OPTIONS] <COMMAND>
```

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
