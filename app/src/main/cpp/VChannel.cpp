//
// Created by 王海龙 on 2022/10/24.
//

#include "VChannel.h"
#include "libavutil/imgutils.h"

VChannel::VChannel(int streamIndex, AVCodecContext *avCodecContext) : BaseChannel(streamIndex,
                                                                                  avCodecContext) {

}

VChannel::~VChannel() noexcept {

}

void VChannel::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}

void *task_video_decode(void *args) {
    auto video_channel = static_cast<VChannel * >(args);
    video_channel->video_decode();
    return nullptr;
}

/**
 * decode 过程
 * 1.压缩包(AVPacket)发送到 FFmpge 缓冲区 avcodec_send_packet
 * 2.释放 packet 因为 FFmpeg 已经保存了一份，可以放心释放
 * 3.从 FFmpge 缓冲区中取 AVFrame avcodec_receive_frame 可能获取失败，因为 FFmpge 缓冲区内部运行慢
 */
void VChannel::video_decode() {
    AVPacket *packet = nullptr;
    while (isPlaying) {
        if (!isPlaying) {
            break;
        }
        int r = queueAVPacket.pop(packet);
        if (r == 0) {//没有取成功，继续取，可能压缩包生成慢，
            continue;
        }
        r = avcodec_send_packet(avCodecContext, packet);
        releaseAVPacket(&packet);
        if (r != 0) {//发送 packet 到 FFmpge 缓冲区失败
            break;
        }

        AVFrame *frame = av_frame_alloc();
        r = avcodec_receive_frame(avCodecContext, frame);
        if (r == AVERROR(EAGAIN)) {// B帧 B帧参考前面成功  B帧参考后面失败   可能是P帧还没有出来， 你等等  你再拿一次 可能就拿到了
            continue;
        } else if (r != 0) {//失败
            break;
        }

        queueAVFrame.push(frame);
    }
    releaseAVPacket(&packet);
}

void *task_video_play(void *args) {
    auto *video_Channel = static_cast<VChannel *>(args);
    video_Channel->video_play();
    return nullptr;
}

/**
 * 获取 avframe 转换 RGBA
 *
 * SWS_FAST_BILINEAR == 很快 可能会模糊
 * SWS_BILINEAR == 适中的算法
 */
void VChannel::video_play() {
    AVFrame *frame = nullptr;
    uint8_t *dst_data[4];// RGBA
    int dst_lineSize[4];// RGBA

    //为 dst_data 申请空间
    av_image_alloc(dst_data, dst_lineSize, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);
    SwsContext *swsContext = sws_getContext(
            //输入
            avCodecContext->width,
            avCodecContext->height,
            avCodecContext->pix_fmt,//自动获取 xxx.mp4文件的 像素格式 例如: yuv420p

            //输出
            avCodecContext->width,
            avCodecContext->height,
            AV_PIX_FMT_RGBA,

            SWS_BILINEAR,
            0, 0, 0
    );

    while (isPlaying) {
        if (!isPlaying) {
            break;
        }
        int r = queueAVFrame.pop(frame);
        if (r == 0) {//如果失败继续等待数据
            continue;
        }

        // YUV 转 RGBA
        sws_scale(swsContext,
                //输入
                  frame->data,
                  frame->linesize, 0,
                  avCodecContext->height,
                //输出
                  dst_data, dst_lineSize
        );
        //渲染画面
        renderCallback(dst_data[0], avCodecContext->width, avCodecContext->height, dst_lineSize[0]);
        //渲染后释放 frame
        releaseAVFrame(&frame);
    }
    releaseAVFrame(&frame);
    isPlaying = 0;
    av_free(&dst_data[0]);
    sws_freeContext(swsContext);
}

void VChannel::play() {
    isPlaying = 1;
    queueAVPacket.setwork(1);//压缩包队列
    queueAVFrame.setwork(1);//原始包队列

    //第一个线程 取出压缩包 解码出原始包 push 到 queueAVFrame 队列中
    pthread_create(&tid_video_decode, nullptr, task_video_decode, this);

    //第二个线程 取出原始包 播放
    pthread_create(&tid_video_play, nullptr, task_video_play, this);
}

void VChannel::stop() {

}




