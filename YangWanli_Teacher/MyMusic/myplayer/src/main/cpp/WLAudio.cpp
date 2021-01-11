//
// Created by jiaqu on 2020/11/25.
//

#include "WLAudio.h"

WLAudio::WLAudio(WLPlayStatus *playStatus, int sample_rate, CallJava *callJava) {
    this->playStatus = playStatus;
    this->sample_Rate = sample_rate;
    this->callJava = callJava;
    this->isCut = false;
    this->end_time = 0;
    this->showPcm = false;

    queue = new WLQueue(playStatus);

    buffer = (uint8_t *) (av_malloc(sample_rate * 2 * 2));

    //音频变速变调功能
    sampleBuffer = static_cast<SAMPLETYPE *>(malloc(sample_rate * 2 * 2));
    soundTouch = new SoundTouch();

    soundTouch->setSampleRate(sample_rate);
    soundTouch->setChannels(2);

    soundTouch->setPitch(pitch);
    soundTouch->setTempo(speed);
//    outFile = fopen("/sdcard/testziliao/outAudio.pcm", "wb");
    LOGI("WLAudio construct pitch: %f speed: %f soundTouch:%p", pitch, speed, soundTouch);

    pthread_mutex_init(&codecMutex, NULL);
}

WLAudio::~WLAudio() {
    pthread_mutex_destroy(&codecMutex);
}

void *decodePlay(void *data) {
    WLAudio *wlAudio = (WLAudio *) data;
    wlAudio->initOpenSLES();
//    pthread_exit(&wlAudio->thread_play);
    return 0;
}

void *pcmCallBack(void *data) {
    WLAudio *wlAudio = (WLAudio *) data;
    wlAudio->bufferQueue = new WLBufferQueue(wlAudio->playStatus);

    while ((wlAudio->playStatus != NULL) && !wlAudio->playStatus->isExit) {
        WLPcmBean *pcmBean = NULL;
        wlAudio->bufferQueue->getBuffer(&pcmBean);
        if (pcmBean == NULL) {
            continue;
        }
        if (pcmBean->buffsize <= wlAudio->defaultPcmSize) {//不用分包
            if (wlAudio->isRecordPcm) {
                wlAudio->callJava->onCallPcmToAAC(CHILD_THREAD, pcmBean->buffer,
                                                  pcmBean->buffsize);
            }
            if (wlAudio->showPcm) {
                wlAudio->callJava->onCallPcmInfo(CHILD_THREAD, pcmBean->buffer,
                                                 pcmBean->buffsize);
            }
        }else{//分包
            int pack_num = pcmBean->buffsize / wlAudio->defaultPcmSize;
            int pack_sub = pcmBean->buffsize % wlAudio->defaultPcmSize;
            for (int i = 0; i < pack_num; ++i) {
                char* bf = (char*)malloc(wlAudio->defaultPcmSize);
                memcpy(bf, pcmBean->buffer + i * wlAudio->defaultPcmSize, wlAudio->defaultPcmSize);
                if (wlAudio->isRecordPcm) {
                    wlAudio->callJava->onCallPcmToAAC(CHILD_THREAD, bf,
                                                      wlAudio->defaultPcmSize);
                }
                if (wlAudio->showPcm) {
                    wlAudio->callJava->onCallPcmInfo(CHILD_THREAD, bf,
                                                     wlAudio->defaultPcmSize);
                }
                free(bf);
                bf = NULL;
            }

            if (pack_sub > 0){
                char* bf = (char*)malloc(pack_sub);
                memcpy(bf, pcmBean->buffer + pack_num * wlAudio->defaultPcmSize, pack_sub);
                if (wlAudio->isRecordPcm) {
                    wlAudio->callJava->onCallPcmToAAC(CHILD_THREAD, bf,
                                                      pack_sub);
                }
                if (wlAudio->showPcm) {
                    wlAudio->callJava->onCallPcmInfo(CHILD_THREAD, bf,
                                                     pack_sub);
                }
                free(bf);
                bf = NULL;
            }
        }

        delete pcmBean;
        pcmBean = NULL;
    }

//    pthread_exit(&wlAudio->pcmCallBackThread);

    return 0;
}

void WLAudio::play() {
    if ((playStatus != NULL) && !playStatus->isExit){
        pthread_create(&thread_play, NULL, decodePlay, this);
        pthread_create(&pcmCallBackThread, NULL, pcmCallBack, this);
    }
}

int WLAudio::resampleAudio(void **pcmbuf) {
    data_size = 0;

    while ((playStatus != NULL) && !playStatus->isExit) {
        if (playStatus->seek) {
            av_usleep(100 * 1000);//100毫秒
            continue;
        }

        if (queue->getQueueSize() == 0) {//加载中
            if (!playStatus->load) {
                playStatus->load = true;
                callJava->onCallLoad(CHILD_THREAD, true);
            }
            av_usleep(100 * 1000);//100毫秒
            continue;
        } else {
            if (playStatus->load) {
                playStatus->load = false;
                callJava->onCallLoad(CHILD_THREAD, false);
            }
        }

        if (readFrameFinish) {
            avPacket = av_packet_alloc();
            if (queue->getAVPacket(avPacket) != 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                continue;
            }

            pthread_mutex_lock(&codecMutex);
            ret = avcodec_send_packet(avCodecContext, avPacket);
            if (ret != 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                pthread_mutex_unlock(&codecMutex);
                continue;
            }
        }

        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == 0) {
            readFrameFinish = false;

            if ((avFrame->channels > 0) && (avFrame->channel_layout == 0)) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if ((avFrame->channels == 0) && (avFrame->channel_layout > 0)) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            SwrContext *swr_ctx;
            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    (AVSampleFormat) (avFrame->format),
                    avFrame->sample_rate,
                    NULL, NULL);
            if (!swr_ctx || (swr_init(swr_ctx) < 0)) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                swr_free(&swr_ctx);
                readFrameFinish = true;

                pthread_mutex_unlock(&codecMutex);
                continue;
            }

            nb = swr_convert(
                    swr_ctx,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) (avFrame->data),
                    avFrame->nb_samples);
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            now_time = avFrame->pts * av_q2d(time_base);
            if (now_time < clock) {//保证clock递增
                now_time = clock;
            }
            clock = now_time;

            *pcmbuf = buffer;

//            fwrite(buffer, data_size, 1, outFile);
            if (LOG_DEBUG) {
//                LOGI("data size %d", data_size);
            }

//            av_packet_free(&avPacket);
//            av_free(avPacket);
//            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swr_ctx);
            swr_ctx = NULL;

            pthread_mutex_unlock(&codecMutex);
            break;
        } else {
            readFrameFinish = true;

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            pthread_mutex_unlock(&codecMutex);
            continue;
        }
    }

//    fclose(outFile);
//    LOGI("fclose the pcm file");
    return data_size;
}

int WLAudio::getSoundTouchData() {
    while (playStatus != NULL && !playStatus->isExit) {
        out_buffer = NULL;

        if (finished) {
            finished = false;

            data_size = resampleAudio(reinterpret_cast<void **>(&out_buffer));//返回当前解码pcm的字节数
            if (data_size > 0) {
                for (int i = 0; i < data_size / 2 + 1; ++i) {
                    sampleBuffer[i] = (out_buffer[i * 2] | ((out_buffer[i * 2 + 1]) << 8));
                }

//                sampleBuffer = (SAMPLETYPE *) out_buffer;

//                LOGI("nb is %d", nb);
                soundTouch->putSamples(sampleBuffer, nb);
                num = soundTouch->receiveSamples(sampleBuffer, data_size / 4);
            } else {//没有输入的pcm数据，刷新变速变调实例的缓冲
                soundTouch->flush();
            }
        }

        if (num == 0) {//没有接收到变速变调返回的数据，继续进行获取操作
            finished = true;
            continue;
        } else {
            if (out_buffer == NULL) {
                num = soundTouch->receiveSamples(sampleBuffer, data_size / 4);
                if (num == 0) {
                    finished = true;
                    continue;
                }
            }

            return num;
        }
    }

    return 0;
}

//给到OpenSLES注册的回调函数，会由OpenSLES主动调用，直接将pcm数据放入OpenSLES中的缓冲队列中进行播放
void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    WLAudio *wlAudio = (WLAudio *) (context);
    if (wlAudio != NULL) {
        int bufferSize = wlAudio->getSoundTouchData();//返回的是当前音频包解码后的PCM的采样点数
        if (bufferSize > 0) {
            wlAudio->clock +=
                    bufferSize / ((double) (wlAudio->sample_Rate * 2 * 2));//累加一下播放一段pcm所耗费的时间

            if (wlAudio->clock - wlAudio->last_time >= 0.1) {
                wlAudio->last_time = wlAudio->clock;
                wlAudio->callJava->onCallTimeInfo(CHILD_THREAD, wlAudio->clock, wlAudio->duration);
            }

//            if (wlAudio->isRecordPcm){
//                wlAudio->callJava->onCallPcmToAAC(CHILD_THREAD, wlAudio->sampleBuffer,
//                                                  bufferSize * 2 * 2);
//            }

            wlAudio->bufferQueue->putBuffer(wlAudio->sampleBuffer, bufferSize * 4);

            wlAudio->callJava->onCallVolumeDB(CHILD_THREAD,
                                              wlAudio->getPCMDB(
                                                      reinterpret_cast<char *>(wlAudio->sampleBuffer),
                                                      bufferSize * 4));

            (*wlAudio->pcmBufferQueue)->Enqueue(wlAudio->pcmBufferQueue, wlAudio->sampleBuffer,
                                                bufferSize * 2 * 2);

            if (wlAudio->isCut) {
//                if (wlAudio->showPcm){
//                    //
//                    wlAudio->callJava->onCallPcmInfo(CHILD_THREAD, wlAudio->sampleBuffer, bufferSize * 2 * 2);
//                }
                if (wlAudio->clock > wlAudio->end_time) {
                    LOGI("裁剪退出...");
                    wlAudio->playStatus->isExit = true;
                }
            }
        }
    }
}

void WLAudio::initOpenSLES() {
    LOGI("initOpenSLES in");
    SLresult result;

    //1.创建接口对象(根据engineObject接口类对象来创建引擎对象,后面的操作都根据这个引擎对象创建相应的操作接口)
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    //2.设置混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};//设置使能混响音效
    const SLboolean mreq[1] = {SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids,
                                              mreq);//通过引擎创建混音器
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);//实现混音器实例
    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,//指定混响音效类型
                                              &outputMixEnvironmentalReverb);//通过混音器设备得到混响音效的实例
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb,
                &reverbSettings);//设置某种指定的混响音效，比如走廊混响
        (void) result;
    }

    //3.创建播放器(录音器)
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};//指定了两个buffer队列
    SLDataFormat_PCM pcm = {//指定设备进行播放的pcm格式参数，按照指定的参数设置进行播放
            SL_DATAFORMAT_PCM,
            2,
            getCurrentSampleRateForOpenSLES(sample_Rate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSource slDataSource = {&android_queue, &pcm};
    SLDataSink audioSink = {&outputMix, NULL};

    const SLInterfaceID ids[4] = {SL_IID_BUFFERQUEUE, SL_IID_MUTESOLO,
                                  SL_IID_VOLUME, SL_IID_PLAYBACKRATE};//指定使能缓存队列和音量操作的接口
    const SLboolean req[4] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    //根据引擎创建音频播放器实例
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource,
                                                &audioSink, 4,
                                                ids, req);

    // 初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    //得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY,
                                     &pcmPlayerPlay);//根据音频播放器实例获取到音频播放接口

    //4.设置缓存队列和回调函数
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE,
                                     &pcmBufferQueue);//根据音频播放器实例获取到音频缓存队列的接口
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);

    //获取音量接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmPlayerVolume);
    setVolume(volumePercent);

    setMute(mute);

    //获取声道接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_MUTESOLO, &pcmPlayerMute);

    //5.设置播放状态
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);

    //6.启动回调函数
    pcmBufferCallBack(pcmBufferQueue, this);

    LOGI("initOpenSLES is end");
}

SLuint32 WLAudio::getCurrentSampleRateForOpenSLES(int sample_rate) {
    SLuint32 rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
            break;
    }
    return rate;
}

void WLAudio::pause() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void WLAudio::resume() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void WLAudio::stop() {
    if (pcmPlayerPlay != NULL) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
}

void WLAudio::release() {
    stop();

    if (bufferQueue != NULL){
        bufferQueue->noticeThread();
        pthread_join(pcmCallBackThread, NULL);
        bufferQueue->release();
        delete bufferQueue;
        bufferQueue = NULL;
    }

    if (queue != NULL){
        queue->noticeQueue();
    }

    pthread_join(thread_play, NULL);
    if (queue != NULL) {
        delete queue;
        queue = NULL;
    }

    if (pcmPlayerObject != NULL) {
        (*pcmPlayerObject)->Destroy(pcmPlayerObject);
        pcmPlayerObject = NULL;
        pcmPlayerPlay = NULL;
        pcmBufferQueue = NULL;
        pcmPlayerVolume = NULL;
        pcmPlayerMute = NULL;
    }

    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    if (out_buffer != NULL) {
        out_buffer = NULL;
    }

    if (soundTouch != NULL) {
        delete soundTouch;
        soundTouch = NULL;
    }

    if (sampleBuffer != NULL) {
        free(sampleBuffer);
        sampleBuffer = NULL;
    }
    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (callJava != NULL) {
        callJava = NULL;
    }
}

void WLAudio::setVolume(int percent) {
    volumePercent = percent;

    if (pcmPlayerVolume != NULL) {
        //下面是为了让音量调节平滑一点
        if (percent > 30) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-20));//percent:100 原声 percent:0 静音
        } else if (percent > 25) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-22));//percent:100 原声 percent:0 静音
        } else if (percent > 20) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-25));//percent:100 原声 percent:0 静音
        } else if (percent > 15) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-28));//percent:100 原声 percent:0 静音
        } else if (percent > 10) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-30));//percent:100 原声 percent:0 静音
        } else if (percent > 5) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-34));//percent:100 原声 percent:0 静音
        } else if (percent > 3) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-37));//percent:100 原声 percent:0 静音
        } else if (percent > 0) {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-40));//percent:100 原声 percent:0 静音
        } else {
            (*pcmPlayerVolume)->SetVolumeLevel(pcmPlayerVolume, (100 - percent) *
                                                                (-100));//percent:100 原声 percent:0 静音
        }
    }
}

void WLAudio::setMute(int mute) {
    this->mute = mute;

    if (pcmPlayerMute != NULL) {
        if (mute == 0) {//rigt 右声道
            (*pcmPlayerMute)->SetChannelMute(pcmPlayerMute, 1, false);//0右声道  1左声道  第三个参数表示是否关闭
            (*pcmPlayerMute)->SetChannelMute(pcmPlayerMute, 0, true);
        } else if (mute == 1) {//left 左声道
            (*pcmPlayerMute)->SetChannelMute(pcmPlayerMute, 1, true);
            (*pcmPlayerMute)->SetChannelMute(pcmPlayerMute, 0, false);
        } else if (mute == 2) {//center 立体声
            (*pcmPlayerMute)->SetChannelMute(pcmPlayerMute, 1, false);
            (*pcmPlayerMute)->SetChannelMute(pcmPlayerMute, 0, false);
        }
    }
}

void WLAudio::setPitch(float pitch) {
    this->pitch = pitch;
    LOGI("WLAudio setPitch: %f soundTouch: %p", this->pitch, soundTouch);
    if (soundTouch != NULL) {
        soundTouch->setPitch(pitch);
    }
}

void WLAudio::setSpeed(float speed) {
    this->speed = speed;
    LOGI("WLAudio setSpeed: %f soundTouch: %p", this->speed, soundTouch);
    if (soundTouch != NULL) {
        soundTouch->setTempo(speed);
    }
}

int WLAudio::getPCMDB(char *pcmdata, size_t pcmsize) {
    int db = 0;
    short int pervalue = 0;
    double sum = 0;

    for (int i = 0; i < pcmsize; i += 2) {
        memcpy(&pervalue, (pcmdata + i), 2);
        sum += abs(pervalue);
    }

    sum = sum / (pcmsize / 2);//求平均
    if (sum > 0) {
        db = (int) 20.0 * log10(sum);
    }

    return db;
}

void WLAudio::startStopRecord(bool start) {
    this->isRecordPcm = start;
}
