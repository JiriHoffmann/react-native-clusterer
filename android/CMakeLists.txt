project(clusterer)
cmake_minimum_required(VERSION 3.9.0)

set(PACKAGE_NAME clusterer)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE cpp_files RELATIVE ${CMAKE_SOURCE_DIR}
  "../cpp/**.cpp"
  "../cpp/**.hpp"
  "cpp-adapter.cpp"
)

# Define C++ library and add all sources
add_library(${PACKAGE_NAME} SHARED ${cpp_files})

# Add Nitrogen specs :)
include(${CMAKE_SOURCE_DIR}/../nitrogen/generated/android/clusterer+autolinking.cmake)

# Set up local includes
include_directories("src/main/cpp" "../cpp")

find_library(LOG_LIB log)

# Link all libraries together
target_link_libraries(
        ${PACKAGE_NAME}
        ${LOG_LIB}
        android # <-- Android core
)
