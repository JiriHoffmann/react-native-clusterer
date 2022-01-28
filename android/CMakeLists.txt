cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME "clusterer")
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 14)


include_directories(
        ${PACKAGE_NAME}
        "${NODE_MODULES_DIR}/react-native/React"
        "${NODE_MODULES_DIR}/react-native/React/Base"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "../cpp"
)

add_library(
        ${PACKAGE_NAME}
        SHARED
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/jsi.cpp"
        ../cpp/react-native-clusterer.cpp
        ../cpp/react-native-clusterer.h
        ../cpp/supercluster.hpp
        ../cpp/clusterer.cpp
        ../cpp/clusterer.h
        ./cpp-adapter.cpp
)

target_link_libraries(
        ${PACKAGE_NAME}
)
