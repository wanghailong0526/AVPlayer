//
// Created by 王海龙 on 2022/10/24.
//

#include "AChannel.h"

AChannel::AChannel(int streamIndex, AVCodecContext *avCodecContext) : BaseChannel(streamIndex,
                                                                                  avCodecContext) {

}

AChannel::~AChannel() noexcept {

}

void AChannel::play() {

}

void AChannel::stop() {

}
