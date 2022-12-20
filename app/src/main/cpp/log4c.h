#ifndef LOG4C_H
#define LOG4C_H

#include <android/log.h>

#define TAG "whl **"

// ...代表可传递任意内容
// __VA_ARGS__ 代表 ...的可变参数

#define LOGD_(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag,  __VA_ARGS__);
#define LOGE_(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag,  __VA_ARGS__);
#define LOGI_(tag, ...) __android_log_print(ANDROID_LOG_INFO, tag,  __VA_ARGS__);

#define LOGD(...) LOGD_(TAG,__VA_ARGS__);
#define LOGE(...) LOGE_(TAG,__VA_ARGS__);
#define LOGI(...) LOGI_(TAG,__VA_ARGS__);

#endif //LOG4C_H
