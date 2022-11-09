//
// Created by 王海龙 on 2022/10/24.
//

#ifndef AVPLAYER_AVCONTROLLER_H
#define AVPLAYER_AVCONTROLLER_H

#include <pthread.h>
#include "AChannel.h"
#include "VChannel.h"
#include "JNICallbackHelper.h"
#include "constant.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"

#ifdef __cplusplus
}
#endif


class AVController {
private:
    char *p_av_path = nullptr;
    AVFormatContext *avFormatContext = nullptr;
    pthread_t tid_prepare;
    AChannel *audio_channel = nullptr;
    VChannel *video_channel = nullptr;
    JNICallbackHelper *jniCallbackHelper = nullptr;
public:
    AVController(const char *pAvPath, JNICallbackHelper *jniCallbackHelper);

    virtual ~AVController();

    void prepare();

    void findStream();

    void play();

};


#endif //AVPLAYER_AVCONTROLLER_H
