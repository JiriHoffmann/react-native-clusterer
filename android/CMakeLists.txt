project(ReactNativeClusterer)
cmake_minimum_required(VERSION 3.9.0)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 17)
set (PACKAGE_NAME "rnclusterer")
set (BUILD_DIR ./build)

file(GLOB RN_CLUSTERER_CPP "../cpp/*.cpp")
file(GLOB RN_CLUSTERER_HPP "../cpp/*.hpp")

# Add headers
include_directories(
  ${PACKAGE_NAME}
  "."

  # rnclusterer headers
  "../cpp"
)

add_library(
        ${PACKAGE_NAME}
        SHARED
        ${RN_CLUSTERER_CPP}
        ${RN_CLUSTERER_HPP}
        ./cpp-adapter.cpp
)

find_package(ReactAndroid REQUIRED CONFIG)
find_library(log-lib log)

# Determine if we're using Android NDK and support --page-align
set(LINK_FLAGS "-Wl,--no-undefined")

if(DEFINED ANDROID_NDK_VERSION)
    string(REGEX MATCH "^([0-9]+)" NDK_MAJOR_VERSION "${ANDROID_NDK_VERSION}")
    if(NDK_MAJOR_VERSION GREATER_EQUAL 25)
        message(STATUS "Enabling --page-align (NDK ${NDK_MAJOR_VERSION} ≥ 25)")
        list(APPEND LINK_FLAGS "-Wl,--page-align")
    else()
        message(STATUS "Skipping --page-align (NDK ${NDK_MAJOR_VERSION} < 25)")
    endif()
else()
    message(STATUS "NDK version unknown; using default linker flags")
endif()

target_link_options(${PACKAGE_NAME} PRIVATE ${LINK_FLAGS})

# Link JNI, JSI, LOG_LIB
target_link_libraries(
  ${PACKAGE_NAME}
  ${log-lib}          # <-- Logcat logger
  ReactAndroid::jsi   # <-- JSI
  android             # <-- Android JNI core
)
