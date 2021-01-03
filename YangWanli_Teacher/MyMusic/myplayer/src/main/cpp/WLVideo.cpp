//
// Created by jiaqu on 2020/12/26.
//
#include "WLVideo.h"

WLVideo::WLVideo(WLPlayStatus *playStatus, CallJava *callJava) {
    this->playStatus = playStatus;
    this->callJava = callJava;
    queue = new WLQueue(playStatus);
}

WLVideo::~WLVideo() {

}

void* playVideo(void* data){
    WLVideo* video = static_cast<WLVideo *>(data);

    while ((video->playStatus != NULL) && !video->playStatus->isExit){
        if (video->playStatus->seek){
            av_usleep(1000*100);
            continue;
        }

        if (video->queue->getQueueSize() == 0){
            if (!video->playStatus->load){
                video->playStatus->load = true;
                video->callJava->onCallLoad(CHILD_THREAD, true);
            }
            av_usleep(1000*100);
            continue;
        }else{
            if (video->playStatus->load){
                video->playStatus->load = false;
                video->callJava->onCallLoad(CHILD_THREAD, false);
            }
        }
        AVPacket *avPacket = av_packet_alloc();
        if (video->queue->getAVPacket(avPacket) != 0){
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        if (avcodec_send_packet(video->avCodecContext, avPacket) != 0){
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        AVFrame *avFrame = av_frame_alloc();
        if (avcodec_receive_frame(video->avCodecContext, avFrame) != 0){
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        LOGI("子线程解码一个AVFrame成功!");
        if (avFrame->format == AV_PIX_FMT_YUV420P){
            LOGI("当前视频是YUV420P格式!");
            //直接渲染
            video->callJava->onCallRenderYUV(CHILD_THREAD,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                avFrame->data[0],
                avFrame->data[1],
                avFrame->data[2]);
        }else{
            LOGI("当前视频不是YUV420P格式，需转换!");
            AVFrame* pFrameYUV420p = av_frame_alloc();
            int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    1);
            uint8_t *buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
            av_image_fill_arrays(pFrameYUV420p->data,
                    pFrameYUV420p->linesize,
                    buffer,
                    AV_PIX_FMT_YUV420P,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    1);

            SwsContext *sws_ctx = sws_getContext(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    video->avCodecContext->pix_fmt,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC, NULL, NULL, NULL);
            if (!sws_ctx){
                av_frame_free(&pFrameYUV420p);
                av_free(pFrameYUV420p);
                av_free(buffer);
                continue;
            }

            sws_scale(sws_ctx,
                    avFrame->data,
                    avFrame->linesize,
                    0,
                    avFrame->height,
                    pFrameYUV420p->data,
                    pFrameYUV420p->linesize);

            //渲染
            video->callJava->onCallRenderYUV(CHILD_THREAD,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                     pFrameYUV420p->data[0],
                     pFrameYUV420p->data[1],
                     pFrameYUV420p->data[2]);

            av_frame_free(&pFrameYUV420p);
            av_free(pFrameYUV420p);
            av_free(buffer);
            sws_freeContext(sws_ctx);
        }

        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }

    pthread_exit(&video->thread_play);
}

void WLVideo::play() {
    pthread_create(&thread_play, NULL, playVideo, this);
}

void WLVideo::release() {
    if (queue != NULL){
        delete queue;
        queue = NULL;
    }

    if (avCodecContext != NULL){
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if (playStatus != NULL){
        playStatus = NULL;
    }

    if (callJava != NULL){
        callJava = NULL;
    }
}
