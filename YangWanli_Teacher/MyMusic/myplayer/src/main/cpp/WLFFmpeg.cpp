//
// Created by jiaqu on 2020/11/25.
//
#include "WLFFmpeg.h"

WLFFmpeg::WLFFmpeg(WLPlayStatus *playStatus, CallJava *calljava, const char *url) {
    this->callJava = calljava;
    strcpy(this->url, url);
    this->playStatus = playStatus;
}

WLFFmpeg::~WLFFmpeg() {

}

void *decodeFFmpeg(void *data) {
    WLFFmpeg *wlfFmpeg = (WLFFmpeg *) (data);
    wlfFmpeg->decodeFFmpegThread();

    pthread_exit(&wlfFmpeg->decodeThread);
}

void WLFFmpeg::prepared() {
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}

void WLFFmpeg::decodeFFmpegThread() {
    av_register_all();
    avformat_network_init();

    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0) {
        if (LOG_DEBUG) {
            LOGE("can not open url: %s", url);
        }
        return;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        if (LOG_DEBUG) {
            LOGE("can not find streams from url: %s", url);
        }
        return;
    }

    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (pWLAudio == NULL) {
                pWLAudio = new WLAudio(playStatus);
                pWLAudio->streamIndex = i;
                pWLAudio->codecPar = pFormatCtx->streams[i]->codecpar;
            }
        }
    }

    AVCodec *avCodec = avcodec_find_decoder(pWLAudio->codecPar->codec_id);
    if (!avCodec) {
        if (LOG_DEBUG) {
            LOGE("can not find deocder");
        }
        return;
    }

    pWLAudio->avCodecContext = avcodec_alloc_context3(avCodec);
    if (!pWLAudio->avCodecContext) {
        if (LOG_DEBUG) {
            LOGE("can not alloc new decoderCtx");
        }
        return;
    }

    if (avcodec_parameters_to_context(pWLAudio->avCodecContext, pWLAudio->codecPar) < 0) {
        if (LOG_DEBUG) {
            LOGE("can not fill decoderCtx");
        }
        return;
    }

    if (avcodec_open2(pWLAudio->avCodecContext, avCodec, 0) != 0) {
        if (LOG_DEBUG) {
            LOGE("can not open audio decoder");
        }
        return;
    }

    LOGI("audio decoder open success!");
    callJava->onCallPrepared(CHILD_THREAD);
}

void WLFFmpeg::start() {
    if (pWLAudio == NULL) {
        if (LOG_DEBUG) {
            LOGE("audio is NULL");
        }
        return;
    }

    pWLAudio->play();

    LOGI("WLFFmpeg is start");
    int count = 0;
    while ((playStatus != NULL) && !playStatus->isExit) {
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(pFormatCtx, avPacket) == 0) {
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
            while ((playStatus != NULL) && !playStatus->isExit){
                if (pWLAudio->queue->getQueueSize() > 0){
                    continue;
                } else{
                    playStatus->isExit = true;
                    break;
                }
            }
        }
    }

    if (LOG_DEBUG){
        LOGI("get packet is over");
    }
}
