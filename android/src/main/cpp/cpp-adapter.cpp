#include <jni.h>
#include "clustererOnLoad.hpp"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return margelo::nitro::clusterer::initialize(vm);
}
