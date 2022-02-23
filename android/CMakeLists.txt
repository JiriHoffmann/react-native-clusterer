cmake_minimum_required(VERSION 3.9.0)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 17)
set (PACKAGE_NAME "rnclusterer")
set (BUILD_DIR ./build)

file(GLOB RN_CLUSTERER_CPP "../cpp/*.cpp")
file(GLOB RN_CLUSTERER_HPP "../cpp/*.hpp")

file(GLOB LIBRN_DIR "${BUILD_DIR}/react-native-0*/jni/${ANDROID_ABI}")

# Add headers
include_directories(
  ${PACKAGE_NAME}
  "${NODE_MODULES_DIR}/react-native/React"
  "${NODE_MODULES_DIR}/react-native/React/Base"
  "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
  "."

  # rnclusterer headers
  "../cpp"
)

# Find JSI on React Native 0.66.x and above
# else manually add JSI on React Native 0.65.x and below
# and add all project source files
if (EXISTS "${LIBRN_DIR}/libjsi.so")
  find_library(
          JSI_LIB
          jsi
          PATHS ${LIBRN_DIR}
          NO_CMAKE_FIND_ROOT_PATH
  )
  add_library(
          ${PACKAGE_NAME}
          SHARED
          ${RN_CLUSTERER_CPP}
          ${RN_CLUSTERER_HPP}
          ./cpp-adapter.cpp
  )
else()
  set (JSI_LIB "")
  add_library(
          ${PACKAGE_NAME}
          SHARED
          ${RN_CLUSTERER_CPP}
          ${RN_CLUSTERER_HPP}
          ${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/jsi.cpp
          ./cpp-adapter.cpp
  )
endif()

# Find JNI
 find_library(
         REACT_NATIVE_JNI_LIB
         reactnativejni
         PATHS ${LIBRN_DIR}
         NO_CMAKE_FIND_ROOT_PATH
 )

# Find LOG_LIB
find_library(
        LOG_LIB
        log
)

# Link JNI, JSI, LOG_LIB
target_link_libraries(
  ${PACKAGE_NAME}
  ${REACT_NATIVE_JNI_LIB}
  ${JSI_LIB}
  ${LOG_LIB}
  android
)
