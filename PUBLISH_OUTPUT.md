# Publishing Output (Client Libraries)

When you generate a C++ API Client SDK or CLI using `cdd-cpp`, you need a strategy to publish it and keep it synchronized.

## Publishing the Generated Client
1. The generated output will include a `CMakeLists.txt` making it a ready-to-use C++ library.
2. Users can consume it directly via CMake `FetchContent` or Git submodules.
3. To publish to a registry (like Conan Center), write a `conanfile.py` that packages the generated C++ headers/sources and dependencies (like `simdjson`).

## Keeping the Client Up-to-Date
To automate updates when the upstream OpenAPI server changes:
1. Create a `.github/workflows/update_client.yml` in your client repository.
2. Set a `schedule` (cronjob) or trigger it via a webhook from the server repository.
3. The action should:
   - Download the latest `openapi.json` from the server.
   - Run `cdd-cpp from_openapi to_sdk -i openapi.json -o src/`
   - Check `git status` for modifications.
   - If changes exist, create a Pull Request or push a new version tag.
