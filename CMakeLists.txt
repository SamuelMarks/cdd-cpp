cmake_minimum_required(VERSION 3.15)
project(cdd_cpp VERSION 0.0.1 LANGUAGES CXX)

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
        set(EXTREME_FLAGS -Wall -Wextra -Wpedantic -Werror -Wno-maybe-uninitialized -Wno-missing-field-initializers -fsanitize=address,undefined -g -fno-omit-frame-pointer)
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
option(USE_VCPKG "Use vcpkg for dependencies" OFF)

if (DEFINED CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
    set(USE_VCPKG ON)
endif()

if (USE_VCPKG)
    find_package(simdjson CONFIG REQUIRED)
    find_package(CURL REQUIRED)
    find_package(PostgreSQL REQUIRED)
else()
    include(FetchContent)
    
    FetchContent_Declare(
        simdjson
        GIT_REPOSITORY https://github.com/simdjson/simdjson.git
        GIT_TAG v3.9.5
    )
    FetchContent_MakeAvailable(simdjson)

    find_package(CURL QUIET)
    if(NOT CURL_FOUND)
        FetchContent_Declare(
            curl
            GIT_REPOSITORY https://github.com/curl/curl.git
            GIT_TAG curl-8_7_1
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

    find_package(PostgreSQL QUIET)
    if(NOT PostgreSQL_FOUND)
        if(POLICY CMP0135)
            cmake_policy(SET CMP0135 NEW)
        endif()
        FetchContent_Declare(
            postgresql
            URL "https://ftp.postgresql.org/pub/source/v15.5/postgresql-15.5.tar.bz2"
        )
        FetchContent_GetProperties(postgresql)
        if(NOT postgresql_POPULATED)
            FetchContent_Populate(postgresql)
            add_library(PostgreSQL::PostgreSQL INTERFACE IMPORTED)
            set_target_properties(PostgreSQL::PostgreSQL PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${postgresql_SOURCE_DIR}/src/interfaces/libpq;${postgresql_SOURCE_DIR}/src/include"
            )
        endif()
    endif()
endif()

# Define Library
set(CDD_LIB_SRCS
    src/functions/parse.cpp src/functions/emit.cpp
    src/classes/parse.cpp src/classes/emit.cpp src/classes/emit_client.cpp src/classes/emit_cli.cpp
    src/docstrings/parse.cpp src/docstrings/emit.cpp
    src/routes/parse.cpp src/routes/emit.cpp
    src/mocks/parse.cpp src/mocks/emit.cpp
    src/utils/http.cpp src/utils/cpp_ast.cpp src/utils/cpp_parser.cpp src/google_discovery/parse.cpp src/openapi/parse.cpp src/openapi/emit.cpp src/openapi/upgraders/upgrader.cpp
    src/orm/emit.cpp
)

add_library(cdd_lib STATIC ${CDD_LIB_SRCS})
target_include_directories(cdd_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
enable_extreme_checks(cdd_lib)

target_link_libraries(cdd_lib PUBLIC simdjson simdjson::simdjson PostgreSQL::PostgreSQL)
if(USE_VCPKG)
    target_link_libraries(cdd_lib PUBLIC CURL::libcurl)
else()
    if(TARGET curl)
        target_link_libraries(cdd_lib PUBLIC curl)
    elseif(TARGET CURL::libcurl)
        target_link_libraries(cdd_lib PUBLIC CURL::libcurl)
    endif()
endif()

# CLI
add_executable(cdd-cpp src/cli/main.cpp src/cli/sync.cpp)
target_link_libraries(cdd-cpp PRIVATE cdd_lib)
enable_extreme_checks(cdd-cpp)

# Tests
add_executable(cdd-tests src/tests/main.cpp src/tests/parse.cpp src/tests/emit.cpp src/tests/upgrader_test.cpp src/tests/google_discovery.cpp src/tests/classes.cpp src/tests/emit_client_test.cpp src/tests/functions.cpp src/tests/docstrings.cpp src/tests/routes.cpp src/tests/mocks.cpp src/tests/http.cpp src/tests/cli.cpp src/tests/orm.cpp)
target_link_libraries(cdd-tests PRIVATE cdd_lib)
enable_extreme_checks(cdd-tests)

# Testing config
enable_testing()
add_test(NAME CDDTests COMMAND cdd-tests)

if(COVERAGE)
    message(STATUS "Adding coverage flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Append ast_test to tests
list(APPEND CDD_TEST_SRCS src/tests/ast_test.cpp)
target_sources(cdd-tests PRIVATE src/tests/ast_test.cpp)

list(APPEND CDD_TEST_SRCS src/tests/sync_test.cpp)
target_sources(cdd-tests PRIVATE src/tests/sync_test.cpp)
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    set_target_properties(cdd-cpp PROPERTIES LINK_FLAGS "-s WASM=1 -s STANDALONE_WASM=1")
endif()
