# ─────────────────────────────────────────────────────────────────────────────
# CMakeLists.txt — Build configuration for ThreadSafeRingBuffer project
#
# What is CMake?
#   CMake does not compile your code directly. It generates the actual build
#   files (like Makefiles) for your platform. You run CMake once to configure,
#   then run the generated build system to compile.
#
# How to use this file:
#   cmake -S . -B build       <- Configure: generates build files in ./build
#   cmake --build build       <- Compile
#   ctest --test-dir build    <- Run all tests
# ─────────────────────────────────────────────────────────────────────────────

cmake_minimum_required(VERSION 3.14)

project(RingBuffer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── ring_buffer library (header-only) ─────────────────────────────────────────
# INTERFACE = no source files, just exposes headers to anything that links to it
add_library(ring_buffer INTERFACE)
target_include_directories(ring_buffer INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ── Main executable ────────────────────────────────────────────────────────────
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE ring_buffer)

# ── Google Test via FetchContent ───────────────────────────────────────────────
# FetchContent downloads googletest automatically at configure time.
# No manual install needed.
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ── Tests ──────────────────────────────────────────────────────────────────────
enable_testing()

add_executable(run_tests tests/test_ring_buffer.cpp)
target_link_libraries(run_tests PRIVATE ring_buffer GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)
