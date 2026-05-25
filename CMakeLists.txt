# ============================================================
# CMakeLists.txt
# QXK24 Universal Kernel — Build System
# ============================================================
# Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
# Created : 2026-05-25
# Repository : https://github.com/qxengine/qxk24
# Path : CMakeLists.txt
# License : Apache 2.0
#
# Description:
# Configures the QXK24 static library, strict compiler settings, and
# GoogleTest-based constitutional proof suite for v1.0.0-alpha.3.
#

cmake_minimum_required(VERSION 3.20)

project(qxk24
    VERSION     1.0.0
    DESCRIPTION "QXK24 Universal Kernel — ALAMTOLOGI Constitutional Kernel"
    LANGUAGES   C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(qxk24_core STATIC
    src/ama/qxk24_ama_pattern.c
    src/level/qxk24_levels.c
    src/direction/qxk24_direction.c
    src/core/qxk24_kernel.c
    src/core/qxk24_kernel_eval.c
    src/core/qxk24_resource.c
    src/core/qxk24_flow.c
    src/core/qxk24_query.c
    src/flow/qxk24_oat.c
    src/flow/qxk24_oass.c
    src/resource/qxk24_resource_base.c
    src/resource/qxk24_governors.c
    src/kernel/qxk24_health.c
    src/kernel/qxk24_report.c
    src/kernel/qxk24_governor.c
)

target_include_directories(qxk24_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(qxk24_core PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    -Wno-unused-parameter
)

target_link_libraries(qxk24_core PRIVATE pthread)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(qxk24_tests
    tests/unit/test_qxk24_ama.cpp
    tests/unit/test_qxk24_levels.cpp
    tests/unit/test_qxk24_flow.cpp
    tests/unit/test_qxk24_governors.cpp
    tests/unit/test_qxk24_health.cpp
    tests/unit/test_qxk24_governor.cpp
    tests/unit/test_qxk24_report.cpp
    tests/integration/test_qxk24_full_cycle.cpp
)

target_link_libraries(qxk24_tests
    PRIVATE qxk24_core
    PRIVATE GTest::gtest_main
)

target_include_directories(qxk24_tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(qxk24_tests
    PROPERTIES TIMEOUT 60
    TEST_PREFIX "QXK24."
    DISCOVERY_TIMEOUT 30
)

message(STATUS "")
message(STATUS "QXK24 Universal Kernel v1.0.0-alpha.3")
message(STATUS "ALAMTOLOGI — QURANIC SCIENCE")
message(STATUS "Founder: Masa Bayu")
message(STATUS "Repository: https://github.com/qxengine/qxk24")
message(STATUS "")
