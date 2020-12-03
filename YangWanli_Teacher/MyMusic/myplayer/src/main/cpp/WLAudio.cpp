//
// Created by jiaqu on 2020/11/25.
//

#include "WLAudio.h"

WLAudio::WLAudio(WLPlayStatus *playStatus, int sample_rate, CallJava* callJava) {
    this->playStatus = playStatus;
    this->sample_Rate = sample_rate;
    this->callJava = callJava;
    queue = new WLQueue(playStatus);

    buffer = (uint8_t *) (av_malloc(sample_rate * 2 * 2));

//    outFile = fopen("/sdcard/testziliao/outAudio.pcm", "wb");
}

WLAudio::~WLAudio() {

}

void *decodePlay(void *data) {
    WLAudio *wlAudio = (WLAudio *) data;
    wlAudio->initOpenSLES();
    pthread_exit(&wlAudio->thread_play);
}

void WLAudio::play() {
    pthread_create(&thread_play, NULL, decodePlay, this);
}

int WLAudio::resampleAudio() {
    while ((playStatus != NULL) && !playStatus->isExit) {
        if (queue->getQueueSize() == 0){
            if (!playStatus->load){
                playStatus->load = true;
                callJava->onCallLoad(CHILD_THREAD, true);
            }
            continue;
        }else{
            if (playStatus->load){
                playStatus->load = false;
                callJava->onCallLoad(CHILD_THREAD, false);
            }
        }

        avPacket = av_packet_alloc();
        if (queue->getAVPacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == 0) {
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
                continue;
            }

            int nb = swr_convert(
                    swr_ctx,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) (avFrame->data),
                    avFrame->nb_samples);
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);

            now_time = avFrame->pts * av_q2d(time_base);
            if (now_time < clock){//保证clock递增
                now_time = clock;
            }
            clock = now_time;

//            fwrite(buffer, data_size, 1, outFile);
            if (LOG_DEBUG) {
//                LOGI("data size %d", data_size);
            }

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swr_ctx);
            swr_ctx = NULL;
            break;
        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }
    }

//    fclose(outFile);
//    LOGI("fclose the pcm file");
    return data_size;
}

//给到OpenSLES注册的回调函数，会由OpenSLES主动调用，直接将pcm数据放入OpenSLES中的缓冲队列中进行播放
void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    WLAudio *wlAudio = (WLAudio *) (context);
    if (wlAudio != NULL) {
        int bufferSize = wlAudio->resampleAudio();
        if (bufferSize > 0) {
            wlAudio->clock += bufferSize / ((double)(wlAudio->sample_Rate*2*2));//累加一下播放一段pcm所耗费的时间

            if (wlAudio->clock - wlAudio->last_time >= 0.1){
                wlAudio->last_time = wlAudio->clock;
                wlAudio->callJava->onCallTimeInfo(CHILD_THREAD, wlAudio->clock, wlAudio->duration);
            }

            (*wlAudio->pcmBufferQueue)->Enqueue(wlAudio->pcmBufferQueue, wlAudio->buffer,
                                                bufferSize);
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

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
                                  SL_IID_VOLUME};//指定使能缓存队列和音量操作的接口
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    //根据引擎创建音频播放器实例
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource,
                                                &audioSink, 3,
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
    if (pcmPlayerObject != NULL){
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void WLAudio::resume() {
    if (pcmPlayerObject != NULL){
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}
