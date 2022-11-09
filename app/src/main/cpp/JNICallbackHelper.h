//
// Created by 王海龙 on 2022/10/24.
//

#ifndef AVPLAYER_JNICALLBACKHELPER_H
#define AVPLAYER_JNICALLBACKHELPER_H


#include <jni.h>

class JNICallbackHelper {
private:
    JavaVM *jvm = nullptr;
    JNIEnv *env = nullptr;
    jobject obj;
    jmethodID jmethodId_prepare;
    jmethodID jmethodId_onError;
public:
    JNICallbackHelper(JavaVM *jvm, JNIEnv *env, jobject obj);

    virtual ~JNICallbackHelper();

    void onPrepared(int thread_mod);

    void onError(int thread_mode, int error_code, char *ffmpegError);

};


#endif //AVPLAYER_JNICALLBACKHELPER_H
