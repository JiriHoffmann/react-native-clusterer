cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME "clusterer")
set (CMAKE_CXX_STANDARD 14)
set (BUILD_DIR ./build)

file (GLOB LIBRN_DIR "${BUILD_DIR}/react-native-0*/jni/${ANDROID_ABI}")

include_directories(
        ${PACKAGE_NAME}
        "${NODE_MODULES_DIR}/react-native/React"
        "${NODE_MODULES_DIR}/react-native/React/Base"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "../cpp"
        "."
)

if (EXISTS "${LIBRN_DIR}/libjsi.so")
    # React Native 0.66.x and above

    find_library(
            JSI_LIB
            jsi
            PATHS ${LIBRN_DIR}
            NO_CMAKE_FIND_ROOT_PATH
    )

    add_library(
            ${PACKAGE_NAME}
            SHARED
            ../cpp/react-native-clusterer.cpp
            ../cpp/react-native-clusterer.h
            ../cpp/supercluster.hpp
            ../cpp/clusterer.cpp
            ../cpp/clusterer.h
            ./cpp-adapter.cpp
    )
else()
    # React Native 0.65.x and below
    set (JSI_LIB "")
    add_library(
            ${PACKAGE_NAME}
            SHARED
            ../../react-native/ReactCommon/jsi/jsi/jsi.cpp
            ../cpp/react-native-clusterer.cpp
            ../cpp/react-native-clusterer.h
            ../cpp/supercluster.hpp
            ../cpp/clusterer.cpp
            ../cpp/clusterer.h
            ./cpp-adapter.cpp
    )
endif()

find_library(
        REACT_NATIVE_JNI_LIB
        reactnativejni
        PATHS ${LIBRN_DIR}
        NO_CMAKE_FIND_ROOT_PATH
)


target_link_libraries(
        ${PACKAGE_NAME}
        ${REACT_NATIVE_JNI_LIB}
        ${JSI_LIB}
        android
)
