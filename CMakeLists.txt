cmake_minimum_required(VERSION 3.5)
project(ballin LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})

include(static_analyzers)
include(enable_vcpkg)
include(remote_clone)

enable_vcpkg()

# set(ballin_LinkerOptions ${ballin_LinkerOptions}
#   . . .
# )

set(ballin_CompilerOptions ${ballin_CompilerOptions}
    -Werror
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wimplicit-fallthrough
)

# find_package(package CONFIG REQUIRED)
# . . .
#
# set(ballin_ExternalLibraries)

add_subdirectory(ballin)
