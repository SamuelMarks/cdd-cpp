@echo off
setlocal

if "%~1"=="" goto build
if "%~1"=="build" goto build
if "%~1"=="test" goto test
if "%~1"=="build_wasm" goto build_wasm
if "%~1"=="clean" goto clean
if "%~1"=="install_deps" goto install_deps

echo Unknown target: %1
exit /b 1

:build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cmake --build build -j4
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
goto :eof

:test
call :build
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
cd build
ctest --output-on-failure
cd ..
goto :eof

:build_wasm
python -c "import sys; sys.path.append('scripts'); import run_pre_commit_checks as r; r.build_wasm()"
goto :eof

:clean
if exist build rmdir /s /q build
if exist build_release rmdir /s /q build_release
if exist build_wasm rmdir /s /q build_wasm
if exist bin rmdir /s /q bin
if exist wasi-sdk rmdir /s /q wasi-sdk
if exist custom_bin rmdir /s /q custom_bin
goto :eof

:install_deps
where dnf >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    sudo dnf install -y cmake gcc-c++ python3 curl git clang llvm llvm-devel libcxx-devel libcxxabi-devel
    goto :eof
)

where apt >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    sudo apt-get update
    sudo apt-get install -y cmake build-essential python3 curl git clang llvm libc++-dev libc++abi-dev
    goto :eof
)

where apk >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    sudo apk add cmake build-base python3 curl git clang llvm libc++-dev
    goto :eof
)

where winget >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    winget install -e --id Kitware.CMake --accept-package-agreements --accept-source-agreements
    winget install -e --id Python.Python.3.11 --accept-package-agreements --accept-source-agreements
    winget install -e --id Git.Git --accept-package-agreements --accept-source-agreements
    winget install -e --id LLVM.LLVM --accept-package-agreements --accept-source-agreements
    goto :eof
)

where scoop >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    scoop install cmake python git llvm gcc
    goto :eof
)

echo No supported package manager found (dnf, apt, apk, winget, scoop)
exit /b 1
