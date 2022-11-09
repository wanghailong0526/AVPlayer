//
// Created by 王海龙 on 2022/10/24.
//

#include "JNICallbackHelper.h"
#include "constant.h"


JNICallbackHelper::JNICallbackHelper(JavaVM *jvm, JNIEnv *env, jobject obj) {
    this->jvm = jvm;
    this->env = env;
    this->obj = env->NewGlobalRef(obj);//跨线程调用需要全局属性
    jclass av_controller_kt_class = env->GetObjectClass(obj);
    this->jmethodId_prepare = env->GetMethodID(av_controller_kt_class, "onPrepared", "()V");
    this->jmethodId_onError = env->GetMethodID(av_controller_kt_class, "onError",
                                               "(ILjava/lang/String;)V");
}

JNICallbackHelper::~JNICallbackHelper() {
    env->DeleteGlobalRef(obj);
    jvm = nullptr;
    obj = nullptr;
    jmethodId_prepare = nullptr;
    env = nullptr;
}

void JNICallbackHelper::onPrepared(int thread_mode) {
    //如果是主线程直接调用方法
    if (thread_mode == THREAD_MAIN) {
        env->CallVoidMethod(obj, jmethodId_prepare);
    } else {//如果是子线程
        JNIEnv *env_child = nullptr;
        jvm->AttachCurrentThread(&env_child, nullptr);
        env_child->CallVoidMethod(obj, jmethodId_prepare);
        jvm->DetachCurrentThread();
    }
}

void JNICallbackHelper::onError(int thread_mode, int error_code, char *ffmpegError) {
    //如果是主线程直接调用方法
    if (thread_mode == THREAD_MAIN) {
        jstring ffmpegError_ = env->NewStringUTF(ffmpegError);
        env->CallVoidMethod(obj, jmethodId_onError, error_code, ffmpegError_);
    } else {//如果是子线程
        JNIEnv *env_child = nullptr;
        jvm->AttachCurrentThread(&env_child, nullptr);
        jstring ffmpegError_ = env_child->NewStringUTF(ffmpegError);
        env_child->CallVoidMethod(obj, jmethodId_onError, error_code, ffmpegError_);
        jvm->DetachCurrentThread();
    }
}

