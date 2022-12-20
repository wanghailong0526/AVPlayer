//
// Created by 王海龙 on 2022/12/5.
//

#ifndef AVPLAYER_BASECHANNEL_H
#define AVPLAYER_BASECHANNEL_H

#include "SafeQueue.h"

#ifdef __cplusplus
extern "C" {
#endif

#include  "libavcodec/avcodec.h"

#ifdef __cplusplus
}
#endif

class BaseChannel {
public:
    int stream_index;//视频或视频的下标
    SafeQueue<AVPacket *> queueAVPacket;//压缩包
    SafeQueue<AVFrame *> queueAVFrame;//原始包
    bool isPlaying = 0;//音视频都有的标记，是否播放
    AVCodecContext *avCodecContext = nullptr;//音视频都需要的解码器上下文

    BaseChannel(int stream_index, AVCodecContext *avCodecContext) : stream_index(stream_index),
                                                                    avCodecContext(avCodecContext) {
        queueAVPacket.setReleaseCallback(releaseAVPacket);
        queueAVFrame.setReleaseCallback(releaseAVFrame);
    }

    virtual ~BaseChannel() {
        queueAVPacket.clear();
        queueAVFrame.clear();
    }

    /**
     * 释放 AVPacket
     * @param **p
     */
    static void releaseAVPacket(AVPacket **p) {
        if (p) {
            av_packet_free(p);
            *p = nullptr;
        }
    }

    /**
     * 释放 AVFrame
     * @param **f
     */
    static void releaseAVFrame(AVFrame **f) {
        if (f) {
            av_frame_free(f);
            *f = nullptr;
        }
    }
};


#endif //AVPLAYER_BASECHANNEL_H
