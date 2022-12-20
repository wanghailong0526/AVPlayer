//
// Created by 王海龙 on 2022/10/24.
//

#ifndef AVPLAYER_VCHANNEL_H
#define AVPLAYER_VCHANNEL_H

#include "BaseChannel.h"

extern "C" {

#include <libavutil/imgutils.h>
#include "libswscale/swscale.h"

}

typedef void (*RenderCallback)(uint8_t *src_data, int width, int height, int src_lineSize);

class VChannel : public BaseChannel {
private:
    pthread_t tid_video_decode;
    pthread_t tid_video_play;
    RenderCallback renderCallback;
public:


    VChannel(int streamIndex, AVCodecContext *avCodecContext);

    virtual ~VChannel();


    void play();

    void video_decode();

    void video_play();

    void stop();

    void setRenderCallback(RenderCallback renderCallback);
};


#endif //AVPLAYER_VCHANNEL_H
