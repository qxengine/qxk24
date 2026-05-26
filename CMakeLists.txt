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
# GoogleTest-based constitutional proof suite for v1.3.0.
#

cmake_minimum_required(VERSION 3.20)

project(qxk24
    VERSION     1.3.0
    DESCRIPTION "QXK24 Universal Kernel — ALAMTOLOGI Faktor Tenaga Kernel"
    LANGUAGES   C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(QXK24_BUILD_TESTS "Build QXK24 GoogleTest proof suite" ON)

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
    src/resource/qxk24_memory_qxengine.c
    src/kernel/qxk24_health.c
    src/kernel/qxk24_report.c
    src/kernel/qxk24_governor.c
    src/bridge/qxk24_qxengine_bridge.c
    src/bridge/qxk24_qxengine_health.c
    src/validation/qxk24_qiubbx.c
    src/memloc/qxk24_memloc_bridge.c
    src/fm/qxk24_fm.c
    src/fm/qxk24_mp.c
    src/fm/qxk24_mdk.c
    src/fm/qxk24_ahli_masa.c
    src/fm/qxk24_usul_masa.c
    src/caliph/qxk24_caliph.c
    src/caliph/qxk24_caliph_command.c
    src/caliph/qxk24_caliph_report.c
    src/ft/qxk24_ft.c
    src/ft/qxk24_padanan.c
    src/ft/qxk24_pasata.c
    src/ft/qxk24_fasa_tenaga.c
    src/ft/qxk24_jisim.c
    src/ft/qxk24_serak.c
    src/ft/qxk24_putar.c
)

target_include_directories(qxk24_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(qxk24_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_compile_options(qxk24_core PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    -Wno-unused-parameter
)

target_link_libraries(qxk24_core PRIVATE pthread)

if(QXK24_BUILD_TESTS)
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
        tests/unit/test_qxk24_memory_qxengine.cpp
        tests/unit/test_qxk24_fm.cpp
        tests/unit/test_qxk24_mp.cpp
        tests/unit/test_qxk24_mdk.cpp
        tests/unit/test_qxk24_ahli_masa.cpp
        tests/unit/test_qxk24_usul_masa.cpp
        tests/unit/test_qxk24_caliph.cpp
        tests/unit/test_qxk24_ft.cpp
        tests/unit/test_qxk24_padanan.cpp
        tests/unit/test_qxk24_pasata.cpp
        tests/unit/test_qxk24_fasa.cpp
        tests/unit/test_qxk24_jisim.cpp
        tests/unit/test_qxk24_serak.cpp
        tests/unit/test_qxk24_putar.cpp
        tests/integration/test_qxk24_full_cycle.cpp
        tests/integration/test_qxk24_qxengine_bridge.cpp
        tests/integration/test_qxk24_qxengine_health.cpp
        tests/integration/test_qxk24_full_stack.cpp
        tests/integration/test_qxk24_caliph_kernel.cpp
        tests/validation/test_qxk24_qiubbx.cpp
        tests/validation/test_qxk24_sovereign.cpp
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
endif()

message(STATUS "")
message(STATUS "QXK24 Universal Kernel v1.3.0")
message(STATUS "Target: SOVEREIGN >= 95.0 maintained")
message(STATUS "ALAMTOLOGI — QURANIC SCIENCE")
message(STATUS "Founder: Masa Bayu")
message(STATUS "Repository: https://github.com/qxengine/qxk24")
message(STATUS "")
