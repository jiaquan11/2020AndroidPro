//
// Created by jiaqu on 2020/12/26.
//
#include "WLVideo.h"

WLVideo::WLVideo(WLPlayStatus *playStatus, CallJava *callJava) {
    this->playStatus = playStatus;
    this->callJava = callJava;
    queue = new WLQueue(playStatus);

    pthread_mutex_init(&codecMutex, NULL);
}

WLVideo::~WLVideo() {
    pthread_mutex_destroy(&codecMutex);
}

void *playVideo(void *data) {
    WLVideo *video = static_cast<WLVideo *>(data);

    while ((video->playStatus != NULL) && !video->playStatus->isExit) {
        if (video->playStatus->seek) {
            av_usleep(1000 * 100);
            continue;
        }

        if (video->playStatus->pause) {
            av_usleep(1000 * 100);
            continue;
        }

        if (video->queue->getQueueSize() == 0) {
            if (!video->playStatus->load) {
                video->playStatus->load = true;
                video->callJava->onCallLoad(CHILD_THREAD, true);
            }
            av_usleep(1000 * 100);
            continue;
        } else {
            if (video->playStatus->load) {
                video->playStatus->load = false;
                video->callJava->onCallLoad(CHILD_THREAD, false);
            }
        }
        AVPacket *avPacket = av_packet_alloc();
        if (video->queue->getAVPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        if (video->codectype == CODEC_MEDIACODEC) {
            LOGI("硬解码视频");
            if (av_bsf_send_packet(video->abs_ctx, avPacket) != 0){
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                continue;
            }
            while (av_bsf_receive_packet(video->abs_ctx, avPacket) == 0){
//                LOGI("av_bsf_receive_packet");
                double diff = video->getFrameDiffTime(NULL, avPacket);
//                LOGI("diff is %lf", diff);

                av_usleep(video->getDelayTime(diff) * 1000000);

                video->callJava->onCallDecodeVPacket(CHILD_THREAD, avPacket->size, avPacket->data);
                av_packet_free(&avPacket);
                av_free(avPacket);
                continue;
            }
            avPacket = NULL;
        } else if (video->codectype == CODEC_YUV) {
            LOGI("软解码视频");
            pthread_mutex_lock(&video->codecMutex);
            if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                pthread_mutex_unlock(&video->codecMutex);
                continue;
            }

            AVFrame *avFrame = av_frame_alloc();
            if (avcodec_receive_frame(video->avCodecContext, avFrame) != 0) {
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;

                pthread_mutex_unlock(&video->codecMutex);
                continue;
            }

            LOGI("子线程解码一个AVFrame成功!");
            if (avFrame->format == AV_PIX_FMT_YUV420P) {
                LOGI("当前视频是YUV420P格式!");
                double diff = video->getFrameDiffTime(avFrame, NULL);
                LOGI("diff is %lf", diff);

                av_usleep(video->getDelayTime(diff) * 1000000);
//            av_usleep(diff * 1000000);
                //直接渲染
                video->callJava->onCallRenderYUV(CHILD_THREAD,
                                                 video->avCodecContext->width,
                                                 video->avCodecContext->height,
                                                 avFrame->data[0],
                                                 avFrame->data[1],
                                                 avFrame->data[2]);
            } else {
                LOGI("当前视频不是YUV420P格式，需转换!");
                AVFrame *pFrameYUV420p = av_frame_alloc();
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
                if (!sws_ctx) {
                    av_frame_free(&pFrameYUV420p);
                    av_free(pFrameYUV420p);
                    av_free(buffer);

                    pthread_mutex_unlock(&video->codecMutex);
                    continue;
                }

                sws_scale(sws_ctx,
                          avFrame->data,
                          avFrame->linesize,
                          0,
                          avFrame->height,
                          pFrameYUV420p->data,
                          pFrameYUV420p->linesize);

                double diff = video->getFrameDiffTime(pFrameYUV420p, NULL);
                LOGI("diff2222 is %lf", diff);

                av_usleep(video->getDelayTime(diff) * 1000000);

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

            pthread_mutex_unlock(&video->codecMutex);
        }
    }

//    pthread_exit(&video->thread_play);
    return 0;
}

void WLVideo::play() {
    if ((playStatus != NULL) && !playStatus->isExit){
        pthread_create(&thread_play, NULL, playVideo, this);
    }
}

void WLVideo::release() {
    if (queue != NULL){
        queue->noticeQueue();
    }

    pthread_join(thread_play, NULL);

    if (queue != NULL) {
        delete queue;
        queue = NULL;
    }

    if (abs_ctx != NULL){
        av_bsf_free(&abs_ctx);
        abs_ctx = NULL;
    }

    if (avCodecContext != NULL) {
        pthread_mutex_lock(&codecMutex);
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
        pthread_mutex_unlock(&codecMutex);
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (callJava != NULL) {
        callJava = NULL;
    }
}

double WLVideo::getFrameDiffTime(AVFrame *avFrame, AVPacket* avPacket) {
    double pts = 0;
    if (avFrame != NULL){
        pts = av_frame_get_best_effort_timestamp(avFrame);
    }
    if (avPacket != NULL){
        pts = avPacket->pts;
    }

    if (pts == AV_NOPTS_VALUE) {
        pts = 0;
    }
    pts *= av_q2d(time_base);
    if (pts > 0) {
        clock = pts;
    }

    double diff = audio->clock - clock;

    LOGI("audio->clock: %lf, video clock: %lf, diff: %lf", audio->clock, clock, diff);
    return diff;
}

double WLVideo::getDelayTime(double diff) {
    if (diff > 0.003) {//音频快 视频慢，减少休眠时间
        delayTime = delayTime * 2 / 3;
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }
    } else if (diff < -0.003) {//视频快，增加休眠时间
        delayTime = delayTime * 3 / 2;
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }
    } else if (diff == 0.003) {

    }

    if (diff >= 0.5) {//音频太快，视频不休眠
        delayTime = 0;
    } else if (diff <= -0.5) {//音频太慢，视频休眠两倍的默认时间
        delayTime = defaultDelayTime * 2;
    }

    if (fabs(diff) >= 10) {//相差很大，基本可以确定没有音频，则视频按照帧率进行播放
        delayTime = defaultDelayTime;
    }

    LOGI("delayTime is %lf", delayTime);
    return delayTime;
}
