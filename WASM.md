# WASM Support

The `cdd-cpp` project supports WebAssembly (WASM) compilation via Emscripten.

## Capabilities

| Feature | Supported | Details |
|---------|-----------|---------|
| WASM | Yes | Implemented using emscripten (`emcmake`). |
| JSON-RPC | Yes | Can run as a server or inside a browser environment. |
| AST Parsing | Yes | C++ Parsing works inside WASM using a unified implementation. |

## Build Instructions

If you have `emsdk` installed one directory above the current working directory, you can build the WASM version by running:

```bash
make build_wasm
```

This will produce the required `.js` and `.wasm` files.
