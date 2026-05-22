# WASM Support

The `cdd-cpp` project supports WebAssembly (WASM) compilation natively via `wasi-sdk`.

## Capabilities

| Feature | Supported | Details |
|---------|-----------|---------|
| WASM | Yes | Implemented using `wasi-sdk` for true cross-platform WASI compatibility. |
| JSON-RPC | Yes | Can run as a server or inside a browser environment (if polyfilled). |
| AST Parsing | Yes | C++ Parsing works inside WASM using a unified implementation. |

## Build Instructions

You can build the WASM version automatically across Windows, Linux, and macOS by running the pre-commit script or a simple python snippet:

```bash
python3 -c "import sys; sys.path.append('scripts'); import run_pre_commit_checks as r; r.build_wasm()"
```

This will download `wasi-sdk` for your platform and produce the `.wasm` file in the `bin` directory.
