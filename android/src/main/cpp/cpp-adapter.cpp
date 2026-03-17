#include <fbjni/fbjni.h>
#include <jni.h>
#include "clustererOnLoad.hpp"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return facebook::jni::initialize(vm, []() {
    margelo::nitro::clusterer::registerAllNatives();
  });
}
