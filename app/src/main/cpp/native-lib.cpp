#include <jni.h>
#include <string>
#include "AVController.h"

JavaVM *jvm = nullptr;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    jvm = vm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_whl_avplayer_AVController_prepareNative(JNIEnv *env, jobject thiz, jstring av_path) {
    const char *p_av_path = env->GetStringUTFChars(av_path, nullptr);
    auto *helper = new JNICallbackHelper(jvm, env, thiz);
    auto *avController = new AVController(p_av_path, helper);
    avController->prepare();
    return reinterpret_cast<jlong>(avController);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_whl_avplayer_AVController_play(JNIEnv *env, jobject thiz, jlong native_obj) {
    auto *avController = reinterpret_cast<AVController *>(native_obj);
    avController->play();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_whl_avplayer_AVController_stop(JNIEnv *env, jobject thiz, jlong native_obj) {
    // TODO: implement stop()
}

extern "C"
JNIEXPORT void JNICALL
Java_com_whl_avplayer_AVController_destory(JNIEnv *env, jobject thiz, jlong native_obj) {
    // TODO: implement destory()
}