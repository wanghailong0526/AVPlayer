package com.whl.avplayer

/**
 *
 * @date   :
 * @author : wanghailong
 *
 * @description:
 *
 */
//单例类
object FFMPGECODE {
    //public static final int CAN_NOT_OPEN_MEDIA = 1
    const val ERR_CAN_NOT_OPEN_MEDIA = 1//打不开媒体
    const val ERR_CAN_NOT_FIND_STREAM = 2//找不到文件流
    const val ERR_CAN_NOT_FIND_DECODER = 3//找不到编解码器
    const val ERR_AVCODEC_ALLOC_CONTEXT_FAIL = 4//创建解码器上下文失败
    const val ERR_AVCODEC_PARAMETERS_TO_CONTEXT_FIAL = 5//AVCodecParameters 复制到 avcodec_context 失败
    const val ERR_AVCODEC_OPEN_FAIL = 6//打开解码器失败
    const val ERR_NO_AUDIO_VIDEO = 7;//没有找到音频和视频流

}