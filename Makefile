.PHONY: build test build_wasm clean

build:
	cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j4

test: build
	cd build && ctest --output-on-failure

build_wasm:
	python3 -c "import sys; sys.path.append('scripts'); import run_pre_commit_checks as r; r.build_wasm()"

clean:
	rm -rf build build_release build_wasm bin wasi-sdk custom_bin
