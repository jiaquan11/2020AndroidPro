//
// Created by jiaqu on 2020/11/25.
//

#ifndef MYMUSIC_WLAUDIO_H
#define MYMUSIC_WLAUDIO_H

#include "WLQueue.h"
#include "WLPlayStatus.h"
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES.h>
#include "CallJava.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};

class WLAudio {
public:
    WLAudio(WLPlayStatus* playStatus, int sample_rate, CallJava* callJava);
    ~WLAudio();

    void play();

    int resampleAudio();

    void initOpenSLES();

    SLuint32 getCurrentSampleRateForOpenSLES(int sample_rate);

    void pause();

    void resume();

public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;

    WLQueue* queue = NULL;
    WLPlayStatus* playStatus = NULL;
    CallJava* callJava = NULL;

    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = -1;
    uint8_t *buffer = NULL;
    int data_size = 0;

    int sample_Rate = 0;

    int duration = 0;
    AVRational time_base;
    double now_time = 0;
    double clock = 0;
    double last_time = 0;

//    FILE* outFile = NULL;

// 引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

//混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;//走廊环境音混响

    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmPlayerVolume = NULL;

//缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;
};
#endif //MYMUSIC_WLAUDIO_H
