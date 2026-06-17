cmake_minimum_required(VERSION 3.15)
project(cdd_cpp VERSION 0.0.2)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-DCPPHTTPLIB_NO_EXCEPTIONS)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(CDD_EXTREME_CHECKS "Enable extreme checks (sanitizers)" ON)

# Extreme checks
if(CDD_EXTREME_CHECKS)
    if(MSVC)
        set(EXTREME_FLAGS /W4 /WX /fsanitize=address)
        set(EXTREME_LINK_FLAGS /fsanitize=address)
    else()
        set(EXTREME_FLAGS -Wall -Wextra -Wpedantic -Werror -Wno-deprecated-literal-operator -Wno-uninitialized -Wno-missing-field-initializers -fsanitize=address,undefined -g -fno-omit-frame-pointer)
        set(EXTREME_LINK_FLAGS -fsanitize=address,undefined)
    endif()
else()
    set(EXTREME_FLAGS "")
    set(EXTREME_LINK_FLAGS "")
endif()

function(enable_extreme_checks target)
    target_compile_options(${target} PRIVATE ${EXTREME_FLAGS})
    target_link_options(${target} PRIVATE ${EXTREME_LINK_FLAGS})
endfunction()

# Options for dependencies

if(CMAKE_SYSTEM_NAME STREQUAL "WASI")
    add_compile_options(-fno-exceptions)
    add_definitions(-DSIMDJSON_EXCEPTIONS=1)
endif()

option(USE_VCPKG "Use vcpkg for dependencies" OFF)

if (DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
    set(USE_VCPKG ON)
endif()

include(FetchContent)

# -----------------
# simdjson
# -----------------
if (USE_VCPKG)
    find_package(simdjson CONFIG REQUIRED)
else()
    FetchContent_Declare(
        simdjson
        URL https://github.com/simdjson/simdjson/archive/refs/tags/v3.9.5.tar.gz
    )
    FetchContent_MakeAvailable(simdjson)
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL "WASI" AND NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    # -----------------
    # CURL
    # -----------------
    if (USE_VCPKG)
        find_package(CURL REQUIRED)
    else()
        find_package(CURL QUIET)
        if(NOT CURL_FOUND)
            FetchContent_Declare(
                curl
                URL https://github.com/curl/curl/archive/refs/tags/curl-8_7_1.tar.gz
            )
            set(BUILD_CURL_EXE OFF CACHE BOOL "Disable curl executable" FORCE)
            set(BUILD_TESTING OFF CACHE BOOL "Disable curl testing" FORCE)
            set(CURL_USE_OPENSSL OFF CACHE BOOL "Disable OpenSSL" FORCE)
            set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
            set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
            set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
            set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
            FetchContent_MakeAvailable(curl)
        endif()
    endif()

    # -----------------
    # PostgreSQL
    # -----------------
    if (USE_VCPKG)
        find_package(PostgreSQL REQUIRED)
    else()
        find_package(PostgreSQL QUIET)
        if(NOT PostgreSQL_FOUND)
            if(POLICY CMP0135)
                cmake_policy(SET CMP0135 NEW)
            endif()
            FetchContent_Declare(
                postgresql
                URL https://github.com/postgres/postgres/archive/refs/tags/REL_15_5.tar.gz
                PATCH_COMMAND ${CMAKE_COMMAND} -E touch CMakeLists.txt
            )
            FetchContent_MakeAvailable(postgresql)
            
            if(NOT TARGET PostgreSQL::PostgreSQL)
                add_library(PostgreSQL::PostgreSQL INTERFACE IMPORTED)
                set_target_properties(PostgreSQL::PostgreSQL PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${postgresql_SOURCE_DIR}/src/interfaces/libpq;${postgresql_SOURCE_DIR}/src/include"
                )
            endif()
        endif()
    endif()
endif()



# Define Library
set(CDD_LIB_SRCS
    src/functions/parse.cpp src/functions/emit.cpp
    src/client_sdk/parse.cpp src/models/emit.cpp src/models/parse.cpp src/client_sdk/emit.cpp src/client_sdk_cli/emit.cpp src/client_sdk_cli/parse.cpp
    src/docstrings/parse.cpp src/docstrings/emit.cpp
    src/server/parse.cpp src/server/emit.cpp
    src/mocks/parse.cpp src/mocks/emit.cpp
    src/utils/http.cpp src/utils/cpp_ast.cpp src/utils/cpp_parser.cpp src/google_discovery/parse.cpp src/openapi/parse.cpp src/openapi/emit.cpp src/openapi/upgraders/upgrader.cpp
    src/orm/emit.cpp src/mcp/parse.cpp src/mcp/emit.cpp
)


if(COVERAGE)
    message(STATUS "Adding coverage flags")
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()
add_library(cdd_lib STATIC ${CDD_LIB_SRCS})
target_include_directories(cdd_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
enable_extreme_checks(cdd_lib)

target_link_libraries(cdd_lib PUBLIC simdjson simdjson::simdjson)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "WASI" AND NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    
    if(USE_VCPKG)
        target_link_libraries(cdd_lib PUBLIC CURL::libcurl)
    else()
        if(TARGET curl)
            target_link_libraries(cdd_lib PUBLIC curl)
        elseif(TARGET CURL::libcurl)
            target_link_libraries(cdd_lib PUBLIC CURL::libcurl)
        endif()
    endif()
endif()

# CLI
add_executable(cdd-cpp src/cli/main.cpp src/cli/sync.cpp src/cli/exceptions_stub.cpp)
target_link_libraries(cdd-cpp PRIVATE cdd_lib)
enable_extreme_checks(cdd-cpp)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "WASI" AND NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    # Tests
    add_executable(cdd-tests src/cli/exceptions_stub.cpp src/tests/client_sdk_cli_test.cpp  src/tests/main.cpp src/tests/parse.cpp src/tests/openapi_parse.cpp src/tests/emit.cpp src/tests/upgrader_test.cpp src/tests/google_discovery.cpp src/tests/models.cpp src/tests/client_sdk_test.cpp src/tests/functions.cpp src/tests/docstrings.cpp src/tests/server.cpp src/tests/mocks.cpp src/tests/http.cpp src/tests/cli.cpp src/tests/orm.cpp src/tests/mcp.cpp)
    target_link_libraries(cdd-tests PRIVATE cdd_lib)
    enable_extreme_checks(cdd-tests)

    # Testing config
    enable_testing()
    add_test(NAME CDDTests COMMAND cdd-tests)

    # Add Python-based integration test for Google Discovery
    find_package(Python COMPONENTS Interpreter)
    if(Python_FOUND)
        add_test(NAME GoogleDiscoveryIntegrationTest COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_google_discovery.py)
    else()
        add_test(NAME GoogleDiscoveryIntegrationTest COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test_google_discovery.py)
    endif()



    # Append ast_test to tests
    list(APPEND CDD_TEST_SRCS src/tests/ast_test.cpp)
    target_sources(cdd-tests PRIVATE src/tests/ast_test.cpp)

    list(APPEND CDD_TEST_SRCS src/tests/sync_test.cpp)
    target_sources(cdd-tests PRIVATE src/tests/sync_test.cpp)
endif()

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    set_target_properties(cdd-cpp PROPERTIES LINK_FLAGS "-s WASM=1 -s STANDALONE_WASM=1")
endif()

set_source_files_properties(
    src/cli/main.cpp
    src/cli/sync.cpp src/cli/exceptions_stub.cpp
    src/client_sdk/emit.cpp
    src/client_sdk/parse.cpp
    src/client_sdk_cli/emit.cpp
    src/client_sdk_cli/parse.cpp
    PROPERTIES COMPILE_OPTIONS -fno-exceptions
)
add_executable(test_simdjson_2 src/tests/test_simdjson_2.cpp src/cli/exceptions_stub.cpp)
target_link_libraries(test_simdjson_2 PRIVATE cdd_lib)
add_test(NAME TestSimdjson2 COMMAND test_simdjson_2)
enable_extreme_checks(test_simdjson_2)
