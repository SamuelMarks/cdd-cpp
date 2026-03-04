@echo off
set BIN_DIR=build
if "%1"=="" goto help
if "%1"=="help" goto help
if "%1"=="all" goto help
if "%1"=="install_base" goto install_base
if "%1"=="install_deps" goto install_deps
if "%1"=="build_docs" goto build_docs
if "%1"=="build" goto build
if "%1"=="test" goto test
if "%1"=="run" goto run
if "%1"=="build_wasm" goto build_wasm
if "%1"=="build_docker" goto build_docker
if "%1"=="run_docker" goto run_docker
goto help

:help
echo Available tasks:
echo   install_base   - install language runtime/tools
echo   install_deps   - install local dependencies
echo   build_docs     - build the API docs and put them in docs
echo   build          - build the CLI binary
echo   test           - run tests locally
echo   run            - run the CLI
echo   build_wasm     - build the WASM version of the CLI
echo   build_docker   - build docker images
echo   run_docker     - run the docker image
goto :eof

:install_base
echo Installing base tools...
goto :eof

:install_deps
echo Installing dependencies...
goto :eof

:build_docs
mkdir docs 2>nul
goto :eof

:build
if not "%2"=="" set BIN_DIR=%2
cmake -B %BIN_DIR% -S . -DCMAKE_BUILD_TYPE=Release
cmake --build %BIN_DIR% --config Release
goto :eof

:test
call :build
%BIN_DIR%\Release\cdd-tests.exe
if errorlevel 1 (
    %BIN_DIR%\cdd-tests.exe
)
goto :eof

:run
call :build
%BIN_DIR%\Release\cdd-cpp.exe %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel 1 (
    %BIN_DIR%\cdd-cpp.exe %2 %3 %4 %5 %6 %7 %8 %9
)
goto :eof

:build_wasm
echo WASM build requires emsdk configured.
goto :eof

:build_docker
docker build -t cdd-cpp-alpine -f alpine.Dockerfile .
docker build -t cdd-cpp-debian -f debian.Dockerfile .
goto :eof

:run_docker
docker run --rm -p 8082:8082 cdd-cpp-alpine serve_json_rpc --port 8082 --listen 0.0.0.0
goto :eof
