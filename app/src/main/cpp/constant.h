//
// Created by 王海龙 on 2022/10/24.
//

#ifndef AVPLAYER_CONSTANT_H
#define AVPLAYER_CONSTANT_H
#define THREAD_MAIN 1
#define THREAD_CHILD 2
// 错误代码
// 打不开视频
#define ERR_CAN_NOT_OPEN_MEDIA 1
// 找不到流媒体
#define ERR_CAN_NOT_FIND_STREAM 2
// 找不到解码器
#define ERR_CAN_NOT_FIND_DECODER 3
// 无法根据解码器创建上下文
#define ERR_AVCODEC_ALLOC_CONTEXT_FAIL 4
// 根据流信息 配置上下文参数失败
#define ERR_AVCODEC_PARAMETERS_TO_CONTEXT_FIAL 5
// 打开解码器失败
#define ERR_AVCODEC_OPEN_FAIL 6
// 没有音视频
#define ERR_NO_AUDIO_VIDEO 7

#endif //AVPLAYER_CONSTANT_H
