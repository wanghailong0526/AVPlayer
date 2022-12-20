//
// Created by 王海龙 on 2022/10/24.
//

#include "AVController.h"

AVController::AVController(const char *pAvPath, JNICallbackHelper *jniCallbackHelper) {
    this->p_av_path = new char[strlen(pAvPath) + 1];//+1 为了末尾的 \0 ; 末尾不加\0，得到的路径会有问题
    strcpy(p_av_path, pAvPath);
    this->jniCallbackHelper = jniCallbackHelper;
}

AVController::~AVController() {
    if (p_av_path) {
        delete p_av_path;
        p_av_path = nullptr;
    }
    if (jniCallbackHelper) {
        delete jniCallbackHelper;
        jniCallbackHelper = nullptr;
    }
}

void *task_prepare(void *args) {
    auto *avController = static_cast<AVController *>(args);
    avController->findStream();
    return nullptr;
}

//启动线程查找 av 流信息
void AVController::prepare() {
    pthread_create(&tid_prepare, nullptr, task_prepare, this);
}

//查找信息流
void AVController::findStream() {
    // TODO 第一步 打开媒体文件
    avFormatContext = avformat_alloc_context();
    AVDictionary *avDictionary = nullptr;
    av_dict_set(&avDictionary, "timeout", "5000000", 0);//单位微秒
    /**
     * AVFormatContext **ps,
     * const char *url, //直播地址或 av 路径
     * ff_const59 AVInputFormat *fmt, 指 mac,windows 系统,摄像头、麦克风 android 用不到
     * AVDictionary **options  参数设置 如:超时时间，
     * r 返回 0_成功 非0_失败
     */
    int r = avformat_open_input(&avFormatContext, p_av_path, nullptr, &avDictionary);
    av_dict_free(&avDictionary);

    if (r) {//TODO 非0失败,通知上层打开文件失败
        if (jniCallbackHelper) {
            jniCallbackHelper->onError(THREAD_CHILD, ERR_CAN_NOT_OPEN_MEDIA, av_err2str(r));
        }
        return;
    }
    //TODO 第二步 查找音频，视频流信息
    r = avformat_find_stream_info(avFormatContext, nullptr);
    if (r < 0) {//失败 通知上层
        if (jniCallbackHelper) {
            jniCallbackHelper->onError(THREAD_CHILD, ERR_CAN_NOT_FIND_STREAM, av_err2str(r));
        }
        return;
    }
    //TODO 第三步 根据流的个数来找音频、视频流; AVFormatContext.nb_streams:媒体文件中的流的个数
    int stream_index = 0;
    for (; stream_index < avFormatContext->nb_streams; ++stream_index) {
        //TODO 第四步 获取媒体流 音频或视频
        AVStream *avstream = avFormatContext->streams[stream_index];
        //TODO 第五步 从上面的流中获取编解码的参数 后面的编码、解码都要用到(如:视频的宽高)
        AVCodecParameters *avcodecParameters = avstream->codecpar;
        //TODO 第六步 根据上面的参数获取编解码器
        AVCodec *avCodec = avcodec_find_decoder(avcodecParameters->codec_id);
        if (!avCodec) {//如果是 NULL 通知上层
            //通知上层 查找解码器失败
            if (jniCallbackHelper) {
                jniCallbackHelper->onError(THREAD_CHILD, ERR_CAN_NOT_FIND_DECODER, "");
            }
            return;
        }
        //TODO 第七步 根据上面的编解码器生成编解码器的上下文
        AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
        if (!avCodecContext) { // pAvCodecContext == NULL_失败  !=NULL_成功
            if (jniCallbackHelper) {
                jniCallbackHelper->onError(THREAD_CHILD, ERR_AVCODEC_ALLOC_CONTEXT_FAIL, "");
            }
            return;
        }
        //TODO 第八步 上面的 pAvCodecContext 是空的，需要把 Parameters 拷贝到 AvCodecContext 里
        r = avcodec_parameters_to_context(avCodecContext, avcodecParameters);
        if (r < 0) {//失败
            if (jniCallbackHelper) {
                jniCallbackHelper->onError(THREAD_CHILD, ERR_AVCODEC_PARAMETERS_TO_CONTEXT_FIAL,
                                           av_err2str(r));
            }
            return;
        }
        //TODO 第九步 打开解码器
        r = avcodec_open2(avCodecContext, avCodec, nullptr);
        if (r) {// 0_成功 非0_失败
            if (jniCallbackHelper) {
                jniCallbackHelper->onError(THREAD_CHILD, ERR_AVCODEC_OPEN_FAIL, av_err2str(r));
            }
            return;
        }
        //TODO 第十步 从编解码参数中获取流的类型 avcodecParameters.codec_type
        if (avcodecParameters->codec_type == AVMediaType::AVMEDIA_TYPE_VIDEO) {//视频流
            video_channel = new VChannel(stream_index, avCodecContext);
            video_channel->setRenderCallback(renderCallback);
        } else if (avcodecParameters->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {//音频流
            audio_channel = new AChannel(stream_index, avCodecContext);
        }
    }

    //TODO 第十一步 如果流中没有音视频流，通知上层
    if (!video_channel && !audio_channel) {
        if (jniCallbackHelper) {
            jniCallbackHelper->onError(THREAD_CHILD, ERR_NO_AUDIO_VIDEO, av_err2str(r));
        }
        return;
    }
    //TODO 第二十步 准备成功 通知上层
    if (jniCallbackHelper) {
        jniCallbackHelper->onPrepared(THREAD_CHILD);//子线程
    }
}

void *task_stream2queue(void *args) {
    auto *avController = static_cast<AVController *>(args);
    avController->avPacketPushQueue();
    return nullptr;
}

/**
 * 把 AVPacket 放到 queue 里去不区分音频频
 */
void AVController::avPacketPushQueue() {
    while (isPlaying) {
        //取出压缩包
        AVPacket *packet = av_packet_alloc();
        int r = av_read_frame(avFormatContext, packet);
        if (r == 0) {//成功
            if (video_channel && video_channel->stream_index == packet->stream_index) {
                video_channel->queueAVPacket.push(packet);
            } else if (audio_channel && audio_channel->stream_index == packet->stream_index) {
                audio_channel->queueAVPacket.push(packet);
            }
        } else if (r == AVERROR_EOF) {//averror_eof 文件尾
            //TODO 表示读完了，不代表播放完成了，先放在这儿
        } else {//否则结束当前循环
            break;
        }
    }
    isPlaying = 0;
    video_channel->stop();
    audio_channel->stop();
}

void AVController::play() {
    isPlaying = 1;//设置为播放状态

    if (video_channel) {
        video_channel->play();//视频播放
    }

    if (audio_channel) {
        audio_channel->play();//音频播放
    }

    pthread_create(&tid_play, nullptr, task_stream2queue, this);
}

void AVController::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}



