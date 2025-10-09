project(ReactNativeClusterer)
cmake_minimum_required(VERSION 3.9.0)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 20)
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

# Link JNI, JSI, LOG_LIB
target_link_libraries(
  ${PACKAGE_NAME}
  ${log-lib}          # <-- Logcat logger
  ReactAndroid::jsi   # <-- JSI
  android             # <-- Android JNI core
)
