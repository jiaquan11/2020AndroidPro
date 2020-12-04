//
// Created by jiaqu on 2020/11/25.
//
#include "WLFFmpeg.h"

WLFFmpeg::WLFFmpeg(WLPlayStatus *playStatus, CallJava *calljava, const char *url) {
    this->callJava = calljava;
    strcpy(this->url, url);
    this->playStatus = playStatus;

    pthread_mutex_init(&init_mutex, NULL);
    pthread_mutex_init(&seek_mutex, NULL);
}

WLFFmpeg::~WLFFmpeg() {
    pthread_mutex_destroy(&init_mutex);
    pthread_mutex_destroy(&seek_mutex);
}

void *decodeFFmpeg(void *data) {
    WLFFmpeg *wlfFmpeg = (WLFFmpeg *) (data);
    wlfFmpeg->decodeFFmpegThread();

    pthread_exit(&wlfFmpeg->decodeThread);
}

void WLFFmpeg::prepared() {
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}

/*
 * 若在avformat_open_input调用时加载网络流很慢，导致阻塞卡死，这个回调函数中返回AVERROR_EOF
 * 会立即退出加载，返回失败
 * */
int avformat_callback(void *ctx) {
    WLFFmpeg *wlfFmpeg = (WLFFmpeg *) (ctx);
    if (wlfFmpeg->playStatus->isExit) {
        return AVERROR_EOF;
    }
    return 0;
}

void WLFFmpeg::decodeFFmpegThread() {
    pthread_mutex_lock(&init_mutex);

    av_register_all();
    avformat_network_init();

    pFormatCtx = avformat_alloc_context();
    pFormatCtx->interrupt_callback.callback = avformat_callback;
    pFormatCtx->interrupt_callback.opaque = this;
    if (avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0) {
        if (LOG_DEBUG) {
            LOGE("can not open url: %s", url);
            callJava->onCallError(CHILD_THREAD, 1001, "can not open url");
        }

        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        if (LOG_DEBUG) {
            LOGE("can not find streams from url: %s", url);
            callJava->onCallError(CHILD_THREAD, 1002, "can not find streams from url");
        }

        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }

    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (pWLAudio == NULL) {
                pWLAudio = new WLAudio(playStatus, pFormatCtx->streams[i]->codecpar->sample_rate,
                                       callJava);
                pWLAudio->streamIndex = i;
                pWLAudio->codecPar = pFormatCtx->streams[i]->codecpar;
                pWLAudio->duration = pFormatCtx->duration / AV_TIME_BASE;
                pWLAudio->time_base = pFormatCtx->streams[i]->time_base;
                duration = pWLAudio->duration;
            }
        }
    }

    AVCodec *avCodec = avcodec_find_decoder(pWLAudio->codecPar->codec_id);
    if (!avCodec) {
        if (LOG_DEBUG) {
            LOGE("can not find deocder");
            callJava->onCallError(CHILD_THREAD, 1003, "can not find deocder");
        }
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }

    pWLAudio->avCodecContext = avcodec_alloc_context3(avCodec);
    if (!pWLAudio->avCodecContext) {
        if (LOG_DEBUG) {
            LOGE("can not alloc new decoderCtx");
            callJava->onCallError(CHILD_THREAD, 1004, "can not alloc new decoderCtx");
        }
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }

    if (avcodec_parameters_to_context(pWLAudio->avCodecContext, pWLAudio->codecPar) < 0) {
        if (LOG_DEBUG) {
            LOGE("can not fill decoderCtx");
            callJava->onCallError(CHILD_THREAD, 1005, "can not fill decoderCtx");
        }
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }

    if (avcodec_open2(pWLAudio->avCodecContext, avCodec, 0) != 0) {
        if (LOG_DEBUG) {
            LOGE("can not open audio decoder");
            callJava->onCallError(CHILD_THREAD, 1006, "can not open audio decoder");
        }
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }

    LOGI("audio decoder open success!");
    callJava->onCallPrepared(CHILD_THREAD);
    pthread_mutex_unlock(&init_mutex);
}

void WLFFmpeg::start() {
    if (pWLAudio == NULL) {
        if (LOG_DEBUG) {
            LOGE("audio is NULL");
            callJava->onCallError(CHILD_THREAD, 1007, "audio is NULL");
        }
        return;
    }

    pWLAudio->play();

    LOGI("WLFFmpeg is start");
    int count = 0;
    while ((playStatus != NULL) && !playStatus->isExit) {
        if (playStatus->seek){
            continue;
        }
        if (pWLAudio->queue->getQueueSize() > 40){
            continue;
        }

        AVPacket *avPacket = av_packet_alloc();

        pthread_mutex_lock(&seek_mutex);
        int ret = av_read_frame(pFormatCtx, avPacket);
        pthread_mutex_unlock(&seek_mutex);
        if (ret == 0) {
            if (avPacket->stream_index == pWLAudio->streamIndex) {
                count++;
                if (LOG_DEBUG) {
//                    LOGI("read audio the packet: %d", count);
                }
                pWLAudio->queue->putAVPacket(avPacket);
            } else {//非音频packet
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            while ((playStatus != NULL) && !playStatus->isExit) {
                if (pWLAudio->queue->getQueueSize() > 0) {
                    continue;
                } else {
                    playStatus->isExit = true;
                    break;
                }
            }
        }
    }

    if (callJava != NULL){
        callJava->onCallComplete(CHILD_THREAD);
    }

    isExit = true;

    if (LOG_DEBUG) {
        LOGI("get packet is over");
    }
}

void WLFFmpeg::pause() {
    if (pWLAudio != NULL) {
        pWLAudio->pause();
    }
}

void WLFFmpeg::resume() {
    if (pWLAudio != NULL) {
        pWLAudio->resume();
    }
}

void WLFFmpeg::seek(int64_t secds) {
    if (duration <= 0){
        return;
    }
    if ((secds >= 0) && (secds <= duration)){
        if (pWLAudio != NULL){
            playStatus->seek = true;
            pWLAudio->queue->clearAvPacket();
            pWLAudio->clock = 0;
            pWLAudio->last_time = 0;

            pthread_mutex_lock(&seek_mutex);
            int64_t rel = secds * AV_TIME_BASE;
            avformat_seek_file(pFormatCtx, -1, INT64_MIN, rel, INT64_MAX, 0);
            pthread_mutex_unlock(&seek_mutex);

            playStatus->seek = false;
        }
    }
}

void WLFFmpeg::release() {
//    if (playStatus->isExit) {
//        return;
//    }
    LOGI("WLFFmpeg release in");
    playStatus->isExit = true;

    pthread_mutex_lock(&init_mutex);
    int sleepCount = 0;
    while (!isExit) {
        if (sleepCount > 1000) {
            isExit = true;
        }
        if (LOG_DEBUG) {
            LOGI("wait ffmpeg exit %d", sleepCount);
        }
        sleepCount++;
        av_usleep(1000 * 10);//10ms
    }

    LOGI("WLFFmpeg release pWLAudio");
    if (pWLAudio != NULL) {
        pWLAudio->release();
        delete pWLAudio;
        pWLAudio = NULL;
    }

    LOGI("WLFFmpeg release pFormatCtx");
    if (pFormatCtx != NULL) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = NULL;
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }
    if (callJava != NULL) {
        callJava = NULL;
    }

    pthread_mutex_unlock(&init_mutex);
    LOGI("WLFFmpeg release end");
}
