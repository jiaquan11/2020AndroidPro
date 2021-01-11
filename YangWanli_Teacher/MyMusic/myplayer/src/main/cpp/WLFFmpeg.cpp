//
// Created by jiaqu on 2020/11/25.
//
#include "WLFFmpeg.h"

WLFFmpeg::WLFFmpeg(WLPlayStatus *playStatus, CallJava *calljava, const char *url) {
    this->callJava = calljava;
    strcpy(this->url, url);
    this->playStatus = playStatus;
    isExit = false;
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

//    pthread_exit(&wlfFmpeg->decodeThread);
    return 0;
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

                callJava->onCallPcmRate(CHILD_THREAD, pWLAudio->sample_Rate, 16, 2);
            }
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (pWLVideo == NULL) {
                pWLVideo = new WLVideo(playStatus, callJava);
                pWLVideo->streamIndex = i;
                pWLVideo->codecPar = pFormatCtx->streams[i]->codecpar;
                pWLVideo->time_base = pFormatCtx->streams[i]->time_base;

                int num = pFormatCtx->streams[i]->avg_frame_rate.num;
                int den = pFormatCtx->streams[i]->avg_frame_rate.den;
                if ((num != 0) && (den != 0)) {
                    int fps = num / den;//比如25/1
                    pWLVideo->defaultDelayTime = 1.0 / fps;
                    LOGI("fps %d, defaultDelayTime: %lf", fps, pWLVideo->defaultDelayTime);
                }
            }
        }
    }

    if (pWLAudio != NULL) {
        getCodecContext(pWLAudio->codecPar, &pWLAudio->avCodecContext);
    }

    if (pWLVideo != NULL) {
        getCodecContext(pWLVideo->codecPar, &pWLVideo->avCodecContext);
    }

    if (callJava != NULL) {
        if ((playStatus != NULL) && !playStatus->isExit) {
            callJava->onCallPrepared(CHILD_THREAD);
        } else {
            isExit = true;
        }
    }

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

    if (pWLVideo == NULL) {//目前要求必须要有视频流
        return;
    }
    supportMediaCodec = false;
    pWLVideo->audio = pWLAudio;

    const char *codecName = ((const AVCodec *) pWLVideo->avCodecContext->codec)->name;
    if (supportMediaCodec = callJava->onCallIsSupportVideo(CHILD_THREAD, codecName)) {
        LOGI("当前设备支持硬解码当前视频!!!");
        if (strcasecmp(codecName, "h264") == 0) {
            bsFilter = av_bsf_get_by_name("h264_mp4toannexb");
        } else if (strcasecmp(codecName, "h265") == 0) {
            bsFilter = av_bsf_get_by_name("hevc_mp4toannexb");
        }
        if (bsFilter == NULL) {
            goto end;
        }
        if (av_bsf_alloc(bsFilter, &pWLVideo->abs_ctx) != 0) {
            supportMediaCodec = false;
            goto end;
        }
        if (avcodec_parameters_copy(pWLVideo->abs_ctx->par_in, pWLVideo->codecPar) < 0) {
            supportMediaCodec = false;
            av_bsf_free(&pWLVideo->abs_ctx);
            pWLVideo->abs_ctx = NULL;
            goto end;
        }
        if (av_bsf_init(pWLVideo->abs_ctx) != 0) {
            supportMediaCodec = false;
            av_bsf_free(&pWLVideo->abs_ctx);
            pWLVideo->abs_ctx = NULL;
            goto end;
        }
        pWLVideo->abs_ctx->time_base_in = pWLVideo->time_base;
    }

    end:
    if (supportMediaCodec) {
        pWLVideo->codectype = CODEC_MEDIACODEC;
        pWLVideo->callJava->onCallinitMediaCodec(CHILD_THREAD,
                                                 codecName,
                                                 pWLVideo->avCodecContext->width,
                                                 pWLVideo->avCodecContext->height,
                                                 pWLVideo->avCodecContext->extradata_size,
                                                 pWLVideo->avCodecContext->extradata_size,
                                                 pWLVideo->avCodecContext->extradata,
                                                 pWLVideo->avCodecContext->extradata);
    }

    pWLAudio->play();
    pWLVideo->play();

    LOGI("WLFFmpeg is start");
    int count = 0;
    while ((playStatus != NULL) && !playStatus->isExit) {
        if (playStatus->seek) {
            av_usleep(100 * 1000);//100毫秒
            LOGI("now is seek continue");
            continue;
        }
        /*对于ape音频文件，一个音频packet可以解码多个frame，因此需要减少缓冲区packet的个数，
         * 避免seek时卡顿,但是对于一个packet对应一个frame的音频文件，这里要改为40
         */
        if (pWLAudio->queue->getQueueSize() > 40) {
            av_usleep(100 * 1000);//100毫秒
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
            } else if (avPacket->stream_index == pWLVideo->streamIndex) {
                pWLVideo->queue->putAVPacket(avPacket);
            } else {//非音频packet
                av_packet_free(&avPacket);
                av_free(avPacket);
            }
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            while ((playStatus != NULL) && !playStatus->isExit) {
                if (pWLAudio->queue->getQueueSize() > 0) {
                    av_usleep(100 * 1000);//100毫秒
                    continue;
                } else {
                    if (!playStatus->seek) {
                        av_usleep(100 * 1000);
                        playStatus->isExit = true;
                        LOGI("playStatus isExit set true");
                    }
                    break;
                }
            }
        }
    }

    if (callJava != NULL) {
        callJava->onCallComplete(CHILD_THREAD);
    }

    isExit = true;

    if (LOG_DEBUG) {
        LOGI("get packet is over");
    }
}

void WLFFmpeg::pause() {
    if (playStatus != NULL) {
        playStatus->pause = true;
    }

    if (pWLAudio != NULL) {
        pWLAudio->pause();
    }
}

void WLFFmpeg::resume() {
    if (playStatus != NULL) {
        playStatus->pause = false;
    }

    if (pWLAudio != NULL) {
        pWLAudio->resume();
    }
}

void WLFFmpeg::seek(int64_t secds) {
    LOGI("WLFFmpeg seek secds: %lld", secds);
    if (duration <= 0) {
        return;
    }
    if ((secds >= 0) && (secds <= duration)) {
        playStatus->seek = true;
        pthread_mutex_lock(&seek_mutex);
        int64_t rel = secds * AV_TIME_BASE;
        avformat_seek_file(pFormatCtx, -1, INT64_MIN, rel, INT64_MAX, 0);
        if (pWLAudio != NULL) {
            pWLAudio->queue->clearAvPacket();
            pWLAudio->clock = 0;
            pWLAudio->last_time = 0;
            pthread_mutex_lock(&pWLAudio->codecMutex);
            avcodec_flush_buffers(pWLAudio->avCodecContext);//清空解码器内部缓冲
            pthread_mutex_unlock(&pWLAudio->codecMutex);
            LOGI("WLFFmpeg pWLAudio seek!!! ");
        }

        if (pWLVideo != NULL) {
            pWLVideo->queue->clearAvPacket();
            pWLVideo->clock = 0;
            pthread_mutex_lock(&pWLVideo->codecMutex);
            avcodec_flush_buffers(pWLVideo->avCodecContext);
            pthread_mutex_unlock(&pWLVideo->codecMutex);
            LOGI("WLFFmpeg pWLVideo seek!!! ");
        }
        pthread_mutex_unlock(&seek_mutex);
        playStatus->seek = false;
        LOGI("WLFFmpeg seek end!");
    }
}

void WLFFmpeg::release() {
//    if (playStatus->isExit) {
//        return;
//    }
    LOGI("WLFFmpeg release in");
    playStatus->isExit = true;

    pthread_join(decodeThread, NULL);

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

    if (pWLAudio != NULL) {
        pWLAudio->release();
        delete pWLAudio;
        pWLAudio = NULL;
        LOGI("WLFFmpeg release pWLAudio");
    }

    if (pWLVideo != NULL) {
        pWLVideo->release();
        delete pWLVideo;
        pWLVideo = NULL;
        LOGI("WLFFmpeg release pWLVideo");
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

void WLFFmpeg::setVolume(int percent) {
    if (pWLAudio != NULL) {
        pWLAudio->setVolume(percent);
    }
}

void WLFFmpeg::setMute(int mute) {
    if (pWLAudio != NULL) {
        pWLAudio->setMute(mute);
    }
}

void WLFFmpeg::setPitch(float pitch) {
    if (pWLAudio != NULL) {
        pWLAudio->setPitch(pitch);
    }
}

void WLFFmpeg::setSpeed(float speed) {
    if (pWLAudio != NULL) {
        pWLAudio->setSpeed(speed);
    }
}

int WLFFmpeg::getSampleRate() {
    if (pWLAudio != NULL) {
        return pWLAudio->avCodecContext->sample_rate;
    }
    return 0;
}

void WLFFmpeg::startStopRecord(bool start) {
    if (pWLAudio != NULL) {
        pWLAudio->startStopRecord(start);
    }
}

bool WLFFmpeg::cutAudioPlay(int start_time, int end_time, bool showPcm) {
    if ((start_time >= 0) && (end_time <= duration) && (start_time < end_time)) {
        pWLAudio->isCut = true;
        pWLAudio->end_time = end_time;
        pWLAudio->showPcm = showPcm;

        seek(start_time);

        return true;
    }
    return false;
}

int WLFFmpeg::getCodecContext(AVCodecParameters *codecPar, AVCodecContext **avCodecContext) {
    AVCodec *avCodec = avcodec_find_decoder(codecPar->codec_id);
    if (!avCodec) {
        if (LOG_DEBUG) {
            LOGE("can not find deocder");
        }
        callJava->onCallError(CHILD_THREAD, 1003, "can not find deocder");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    *avCodecContext = avcodec_alloc_context3(avCodec);
    if (!*avCodecContext) {
        if (LOG_DEBUG) {
            LOGE("can not alloc new decoderCtx");
        }
        callJava->onCallError(CHILD_THREAD, 1004, "can not alloc new decoderCtx");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    if (avcodec_parameters_to_context(*avCodecContext, codecPar) < 0) {
        if (LOG_DEBUG) {
            LOGE("can not fill decoderCtx");
        }
        callJava->onCallError(CHILD_THREAD, 1005, "can not fill decoderCtx");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    if (avcodec_open2(*avCodecContext, avCodec, 0) != 0) {
        if (LOG_DEBUG) {
            LOGE("can not open audio decoder");
        }
        callJava->onCallError(CHILD_THREAD, 1006, "can not open audio decoder");
        isExit = true;
        pthread_mutex_unlock(&init_mutex);
        return -1;
    }

    LOGI("decoder open success!");
    return 0;
}
