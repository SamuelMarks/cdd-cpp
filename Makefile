.PHONY: install_base install_deps build_docs build test run help all build_wasm build_docker run_docker

DOCS_DIR ?= docs
BIN_DIR ?= build

all: help

help:
	@echo "Available tasks:"
	@echo "  install_base   - install language runtime/tools"
	@echo "  install_deps   - install local dependencies (vcpkg if used)"
	@echo "  build_docs     - build the API docs and put them in DOCS_DIR"
	@echo "  build          - build the CLI binary"
	@echo "  test           - run tests locally"
	@echo "  run            - run the CLI (builds if not present). pass args like: make run ARGS='--version'"
	@echo "  build_wasm     - build the WASM version of the CLI"
	@echo "  build_docker   - build docker images (alpine and debian)"
	@echo "  run_docker     - run the docker image (alpine) for JSON RPC"
	@echo "  help / all     - show this help text"

install_base:
	sudo apt-get update && sudo apt-get install -y cmake build-essential python3 curl git

install_deps:
	# Dependencies handled by cmake/fetchcontent or vcpkg

build_docs:
	mkdir -p $(DOCS_DIR)
	# python tools/doc_coverage.py

build:
	cmake -B $(BIN_DIR) -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build $(BIN_DIR) -j$$(nproc)

test: build
	./$(BIN_DIR)/cdd-tests

run: build
	./$(BIN_DIR)/cdd-cpp $(ARGS)

build_wasm:
	@echo "Building WASM via wasi-sdk..."
	@if [ ! -d "wasi-sdk" ]; then \
		OS_NAME=$$(uname -s | tr A-Z a-z); \
		ARCH_NAME=$$(uname -m); \
		if [ "$$OS_NAME" = "darwin" ]; then WASI_OS="macos"; else WASI_OS="linux"; fi; \
		if [ "$$ARCH_NAME" = "x86_64" ] || [ "$$ARCH_NAME" = "amd64" ]; then WASI_ARCH="x86_64"; else WASI_ARCH="arm64"; fi; \
		curl -L -O "https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-24/wasi-sdk-24.0-$${WASI_ARCH}-$${WASI_OS}.tar.gz"; \
		tar xf wasi-sdk-24.0-*-$${WASI_OS}.tar.gz; \
		rm wasi-sdk-24.0-*-$${WASI_OS}.tar.gz; \
		mv wasi-sdk-24.0* wasi-sdk; \
	fi
	@sed -i.bak 's/VERSION 3.4.0/VERSION 3.11/g' wasi-sdk/share/cmake/wasi-sdk.cmake || true
	rm -rf build_wasm && mkdir -p build_wasm && \
	cd build_wasm && cmake .. -DCMAKE_TOOLCHAIN_FILE=../wasi-sdk/share/cmake/wasi-sdk.cmake -DCDD_EXTREME_CHECKS=OFF -DSIMDJSON_ENABLE_THREADS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fno-exceptions -DSIMDJSON_EXCEPTIONS=1" -DCMAKE_C_FLAGS="-fno-exceptions" && \
	python3 ../patch_simdjson_simple.py && \
	$(MAKE) cdd-cpp
	mkdir -p bin
	cp build_wasm/cdd-cpp bin/cdd-cpp.wasm

build_docker:
	docker build -t cdd-cpp-alpine -f alpine.Dockerfile .
	docker build -t cdd-cpp-debian -f debian.Dockerfile .

run_docker:
	docker run --rm -p 8082:8082 cdd-cpp-alpine serve_json_rpc --port 8082 --listen 0.0.0.0
