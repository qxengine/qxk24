# ============================================================
# CMakeLists.txt
# QXK24 Platform - iOS and Android Bridge Build Wiring
# ============================================================
# Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
# Created : 2026-05-25
# Repository : https://github.com/qxengine/qxk24
# Path : platform/CMakeLists.txt
# License : Apache 2.0
# ============================================================

cmake_minimum_required(VERSION 3.22)
project(qxk24_platform VERSION 1.0.0 LANGUAGES C)

set(QXK24_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/..
                 ${CMAKE_BINARY_DIR}/qxk24_core)

if(ANDROID)
    add_library(qxk24 SHARED
        android/qxk24_jni.c
    )

    target_include_directories(qxk24 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )

    target_link_libraries(qxk24
        qxk24_core
        android
        log
    )

    target_compile_options(qxk24 PRIVATE
        -std=c11
        -Wall
        -Wextra
        -Wpedantic
    )

    message(STATUS "QXK24 Android JNI bridge configured")
endif()

if(APPLE AND NOT ANDROID)
    message(STATUS "QXK24 iOS bridge: use platform/ios/QXK24.podspec")
endif()
