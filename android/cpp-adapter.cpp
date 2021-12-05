#include <jni.h>
#include "react-native-clusterer.h"

extern "C" JNIEXPORT void JNICALL
Java_com_reactnativeclusterer_ClustererModule_nativeInstall(JNIEnv *env, jobject thiz, jlong jsi)
{
    auto runtime = reinterpret_cast<facebook::jsi::Runtime *>(jsi);

    if (runtime)
    {
        example::install(*runtime);
    }
}
