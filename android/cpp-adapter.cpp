#include <jni.h>
#include "react-native-clusterer.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativeclusterer_ClustererModule_initialize(JNIEnv *env, jclass clazz, jlong jsi) {
    clusterer::install(*reinterpret_cast<facebook::jsi::Runtime *>(jsi));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativeclusterer_ClustererModule_destruct(JNIEnv *env, jclass clazz) {
    clusterer::cleanup();
}
