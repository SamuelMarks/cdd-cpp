@echo off
setlocal enabledelayedexpansion

if "%~1"=="install_base" goto install_base
if "%~1"=="install_deps" goto install_deps
if "%~1"=="build_docs" goto build_docs
if "%~1"=="build" goto build
if "%~1"=="test" goto test
if "%~1"=="run" goto run
if "%~1"=="build_wasm" goto build_wasm
if "%~1"=="build_docker" goto build_docker
if "%~1"=="run_docker" goto run_docker
if "%~1"=="help" goto help
if "%~1"=="all" goto help
if "%~1"=="" goto help

:help
echo Available tasks:
echo   make.bat install_base : install language runtime
echo   make.bat install_deps : install local dependencies
echo   make.bat build_docs [dir] : build API docs into dir (default docs)
echo   make.bat build [dir]      : build CLI binary into dir (default custom_bin)
echo   make.bat test         : run tests locally
echo   make.bat run [...]    : run CLI with arguments
echo   make.bat build_wasm   : build the project to WASM
echo   make.bat build_docker : build docker images
echo   make.bat run_docker   : run docker test
goto :eof

:install_base
echo Please install Visual Studio with C++ development tools, CMake, and Doxygen.
goto :eof

:install_deps
echo Dependencies will be fetched during build via CMake.
goto :eof

:build_docs
set DOCS_DIR=%~2
if "%DOCS_DIR%"=="" set DOCS_DIR=docs
if not exist "%DOCS_DIR%" mkdir "%DOCS_DIR%"
echo PROJECT_NAME = cdd-cpp > "%DOCS_DIR%\Doxyfile"
echo INPUT = src >> "%DOCS_DIR%\Doxyfile"
echo OUTPUT_DIRECTORY = %DOCS_DIR% >> "%DOCS_DIR%\Doxyfile"
echo GENERATE_HTML = YES >> "%DOCS_DIR%\Doxyfile"
echo GENERATE_LATEX = NO >> "%DOCS_DIR%\Doxyfile"
echo OPTIMIZE_OUTPUT_FOR_C = YES >> "%DOCS_DIR%\Doxyfile"
echo EXTRACT_ALL = YES >> "%DOCS_DIR%\Doxyfile"
doxygen "%DOCS_DIR%\Doxyfile"
goto :eof

:build
set BIN_DIR=%~2
if "%BIN_DIR%"=="" set BIN_DIR=custom_bin
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
copy build\Release\cdd-cpp.exe "%BIN_DIR%\" > nul 2>&1
if errorlevel 1 copy build\cdd-cpp.exe "%BIN_DIR%\" > nul 2>&1
goto :eof

:test
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
cmake --build build --config Debug --parallel 4
cd build && ctest -C Debug --output-on-failure
goto :eof

:run
set BIN_DIR=custom_bin
if exist "%BIN_DIR%\cdd-cpp.exe" goto skip_build
call make.bat build %BIN_DIR%
:skip_build
set args=
shift
:loop
if "%~1"=="" goto run_cmd
set args=%args% %1
shift
goto loop
:run_cmd
"%BIN_DIR%\cdd-cpp.exe" %args%
goto :eof

:build_wasm
set BIN_DIR=%~2
if "%BIN_DIR%"=="" set BIN_DIR=custom_bin
call ..\emsdk\emsdk_env.bat
emcmake cmake -B build_wasm -S . -DCMAKE_BUILD_TYPE=Release
emmake cmake --build build_wasm --config Release --parallel 4
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
copy build_wasm\Release\cdd-cpp.* "%BIN_DIR%\" > nul 2>&1
if errorlevel 1 copy build_wasm\cdd-cpp.* "%BIN_DIR%\" > nul 2>&1
goto :eof
clean:
	rmdir /s /q build build_wasm custom_bin docs 2>nul

:build_docker
docker build -t cdd-cpp-alpine -f alpine.Dockerfile .
docker build -t cdd-cpp-debian -f debian.Dockerfile .
goto :eof

:run_docker
call make.bat build_docker
docker run -d -p 8082:8082 --name cdd-cpp-test cdd-cpp-alpine
timeout /t 2 /nobreak
curl -X POST http://localhost:8082/rpc -H "Content-Type: application/json" -d "{\"jsonrpc\": \"2.0\", \"method\": \"--version\", \"id\": 1}"
docker stop cdd-cpp-test
docker rm cdd-cpp-test
docker rmi cdd-cpp-alpine cdd-cpp-debian
goto :eof
