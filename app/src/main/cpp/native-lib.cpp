#include <jni.h>
#include <string>
#include "AVController.h"
#include <android/native_window_jni.h>
#include "log4c.h"

extern "C" {
#include <libavutil/avutil.h>
}

JavaVM *jvm = nullptr;
ANativeWindow *window = nullptr;
pthread_mutex_t mutex = PTHREAD_COND_INITIALIZER;// mutex 静态初始化

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    jvm = vm;
    return JNI_VERSION_1_6;
}

/**
 * 函数指针实现，实现渲染画面
 * @param src_data
 * @param width
 * @param height
 * @param src_lineSize
 */
void renderCallback(uint8_t *src_data, int width, int height, int src_lineSize) {
    pthread_mutex_lock(&mutex);

    //出现问题后 释放锁 防止死锁
    if (!window) {
        pthread_mutex_unlock(&mutex);
    }
    //设置 window 宽高和显示格式
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
    //缓冲区
    ANativeWindow_Buffer windowBuffer;

    //如果 window lock 失败,解锁然后返回
    if (ANativeWindow_lock(window, &windowBuffer, 0)) {// 0_成功 非0_失败
        ANativeWindow_release(window);
        window = 0;
        pthread_mutex_unlock(&mutex);
        return;
    }

    //开始渲染 RGBA 数据,需要字节对齐
    uint8_t *dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    // dst_lineSize 是一行多少个字节, windowBuffer.stride 是一行多少个像素数据, RGBA 占4个字节
    int dst_lineSize = windowBuffer.stride * 4;
    //一行一行拷贝数据
    int i = 0;
    for (; i < windowBuffer.height; ++i) {
        memcpy(dst_data + i * dst_lineSize, src_data + i * src_lineSize, dst_lineSize);
        LOGD_("whl ** ", "dst_linesize:%d  src_linesize:%d\n", dst_lineSize, src_lineSize);
    }
    //绘制画面
    ANativeWindow_unlockAndPost(window);

    pthread_mutex_unlock(&mutex);

}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_whl_avplayer_AVController_prepareNative(JNIEnv *env, jobject thiz, jstring av_path) {
    const char *p_av_path = env->GetStringUTFChars(av_path, nullptr);
    auto *helper = new JNICallbackHelper(jvm, env, thiz);
    auto *avController = new AVController(p_av_path, helper);
    avController->setRenderCallback(renderCallback);
    avController->prepare();
    env->ReleaseStringUTFChars(av_path, p_av_path);
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

extern "C"
JNIEXPORT void JNICALL
Java_com_whl_avplayer_AVController_setSurfaceNative(JNIEnv *env, jobject thiz, jobject surface) {
    pthread_mutex_lock(&mutex);

    //释放之前显示的窗口
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    window = ANativeWindow_fromSurface(env, surface);

    pthread_mutex_unlock(&mutex);
}