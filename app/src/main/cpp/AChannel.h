//
// Created by 王海龙 on 2022/10/24.
//

#ifndef AVPLAYER_ACHANNEL_H
#define AVPLAYER_ACHANNEL_H

#include "BaseChannel.h"

class AChannel : public BaseChannel {
public:
    AChannel(int streamIndex, AVCodecContext *avCodecContext);
    ~AChannel();

    void play();
    void stop();
};


#endif //AVPLAYER_ACHANNEL_H
