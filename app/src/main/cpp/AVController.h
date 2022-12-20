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
    pthread_t tid_prepare;
    pthread_t tid_play;
    AChannel *audio_channel = nullptr;
    VChannel *video_channel = nullptr;
    AVFormatContext *avFormatContext = nullptr;
    JNICallbackHelper *jniCallbackHelper = nullptr;
    bool isPlaying = 0;
    RenderCallback renderCallback;
public:
    AVController(const char *pAvPath, JNICallbackHelper *jniCallbackHelper);

    virtual ~AVController();

    void prepare();

    void findStream();

    void play();

    void avPacketPushQueue();

    void setRenderCallback(RenderCallback renderCallback);

};


#endif //AVPLAYER_AVCONTROLLER_H
