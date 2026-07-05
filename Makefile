.PHONY: build test build_wasm clean install_deps

build:
	cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j4

test: build
	cd build && ctest --output-on-failure

build_wasm:
	python3 -c "import sys; sys.path.append('scripts'); import run_pre_commit_checks as r; r.build_wasm()"

clean:
	rm -rf build build_release build_wasm bin wasi-sdk custom_bin

install_deps:
	@if command -v dnf >/dev/null 2>&1; then \
		sudo dnf install -y cmake gcc-c++ python3 curl git clang llvm llvm-devel libcxx-devel libcxxabi-devel; \
	elif command -v apt >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y cmake build-essential python3 curl git clang llvm libc++-dev libc++abi-dev; \
	elif command -v apk >/dev/null 2>&1; then \
		sudo apk add cmake build-base python3 curl git clang llvm libc++-dev; \
	elif command -v winget >/dev/null 2>&1; then \
		winget install -e --id Kitware.CMake --accept-package-agreements --accept-source-agreements && \
		winget install -e --id Python.Python.3.11 --accept-package-agreements --accept-source-agreements && \
		winget install -e --id Git.Git --accept-package-agreements --accept-source-agreements && \
		winget install -e --id LLVM.LLVM --accept-package-agreements --accept-source-agreements; \
	elif command -v scoop >/dev/null 2>&1; then \
		scoop install cmake python git llvm gcc; \
	else \
		echo "No supported package manager found (dnf, apt, apk, winget, scoop)"; \
		exit 1; \
	fi
