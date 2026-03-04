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
	if [ -d "../emsdk" ]; then \
		cd ../emsdk && . ./emsdk_env.sh && cd ../cdd-cpp && \
		emcmake cmake -B build_wasm -S . -DCMAKE_BUILD_TYPE=Release && \
		cmake --build build_wasm -j$$(nproc); \
	else \
		echo "emsdk not found in ../emsdk"; \
		exit 1; \
	fi

build_docker:
	docker build -t cdd-cpp-alpine -f alpine.Dockerfile .
	docker build -t cdd-cpp-debian -f debian.Dockerfile .

run_docker:
	docker run --rm -p 8082:8082 cdd-cpp-alpine serve_json_rpc --port 8082 --listen 0.0.0.0