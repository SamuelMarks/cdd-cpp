.PHONY: install_base install_deps build_docs build test run help all default build_wasm

default: help
all: help

ifeq (build_docs,$(firstword $(MAKECMDGOALS)))
  DOCS_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(DOCS_ARGS):;@:)
endif

ifeq (build,$(firstword $(MAKECMDGOALS)))
  BUILD_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(BUILD_ARGS):;@:)
endif

ifeq (run,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

help:
	@echo "Available tasks:"
	@echo "  make install_base : install language runtime (gcc, cmake, pkg-config)"
	@echo "  make install_deps : install local dependencies"
	@echo "  make build_docs [dir] : build the API docs and put them in 'dir' directory (default: docs)"
	@echo "  make build [dir]      : build the CLI binary into 'dir' (default: custom_bin)"
	@echo "  make test         : run tests locally"
	@echo "  make run [...]    : run the CLI with optional args"\n	@echo "  make build_wasm   : build the project to WASM"

install_base:
	sudo apt-get update && sudo apt-get install -y build-essential cmake pkg-config curl doxygen

install_deps:
	@echo "Dependencies will be fetched during build."

DOCS_DIR = $(if $(DOCS_ARGS),$(word 1,$(DOCS_ARGS)),docs)
build_docs:
	@mkdir -p $(DOCS_DIR)
	@echo "PROJECT_NAME = cdd-cpp" > $(DOCS_DIR)/Doxyfile
	@echo "INPUT = src" >> $(DOCS_DIR)/Doxyfile
	@echo "OUTPUT_DIRECTORY = $(DOCS_DIR)" >> $(DOCS_DIR)/Doxyfile
	@echo "GENERATE_HTML = YES" >> $(DOCS_DIR)/Doxyfile
	@echo "GENERATE_LATEX = NO" >> $(DOCS_DIR)/Doxyfile
	@echo "OPTIMIZE_OUTPUT_FOR_C = YES" >> $(DOCS_DIR)/Doxyfile
	@echo "EXTRACT_ALL = YES" >> $(DOCS_DIR)/Doxyfile
	doxygen $(DOCS_DIR)/Doxyfile

BIN_DIR = $(if $(BUILD_ARGS),$(word 1,$(BUILD_ARGS)),custom_bin)
build:
	cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j4
	@mkdir -p $(BIN_DIR)
	cp build/cdd-cpp $(BIN_DIR)/

test:
	cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
	cmake --build build -j4
	cd build && ctest --output-on-failure

run: build
	./$(BIN_DIR)/cdd-cpp $(RUN_ARGS)

build_wasm:
	@bash -c "source ../emsdk/emsdk_env.sh && emcmake cmake -B build_wasm -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=\"-s WASM=1 -s EXPORTED_RUNTIME_METHODS=[ccall,cwrap]\" && emmake cmake --build build_wasm -j4"
	@mkdir -p $(BIN_DIR)
	@cp build_wasm/cdd-cpp* $(BIN_DIR)/ 2>/dev/null || true
clean:
	rm -rf build build_wasm custom_bin docs
