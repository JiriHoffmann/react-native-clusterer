cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME "cpp")
set (BUILD_DIR ./build)

include_directories(
        ${PACKAGE_NAME}
        "${NODE_MODULES_DIR}/react-native/React"
        "${NODE_MODULES_DIR}/react-native/React/Base"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "../cpp"
)

file (GLOB LIBRN_DIR "${BUILD_DIR}/react-native-0*/jni/${ANDROID_ABI}")

if (JSI_LIB)
  # React Native 0.66.x and above
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

  find_library(
          JSI_LIB
          jsi
          PATHS ${LIBRN_DIR}
          NO_CMAKE_FIND_ROOT_PATH
  )
else()
  # React Native 0.65.x and below
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
        LOG_LIB
        log
)

#find_library(
#        REACT_NATIVE_JNI_LIB
#        reactnativejni
#        PATHS ${LIBRN_DIR}
#        NO_CMAKE_FIND_ROOT_PATH
#)

set_target_properties(
        ${PACKAGE_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
        ${PACKAGE_NAME}
        ${LOG_LIB}
#        ${REACT_NATIVE_JNI_LIB}
        ${JSI_LIB}
        android
)
