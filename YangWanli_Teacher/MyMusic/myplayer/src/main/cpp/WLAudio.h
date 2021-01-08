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
#include "soundtouch/include/SoundTouch.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "WLBufferQueue.h"
#include "WLPcmBean.h"

using namespace soundtouch;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>
};

class WLAudio {
public:
    WLAudio(WLPlayStatus *playStatus, int sample_rate, CallJava *callJava);

    ~WLAudio();

    void play();

    int resampleAudio(void **pcmbuf);

    void initOpenSLES();

    SLuint32 getCurrentSampleRateForOpenSLES(int sample_rate);

    void pause();

    void resume();

    void stop();

    void release();

    void setVolume(int percent);

    void setMute(int mute);

    int getSoundTouchData();

    void setPitch(float pitch);

    void setSpeed(float speed);

    int getPCMDB(char *pcmdata, size_t pcmsize);

    void startStopRecord(bool start);

public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;

    WLQueue *queue = NULL;
    WLPlayStatus *playStatus = NULL;
    CallJava *callJava = NULL;

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

    int volumePercent = 100;
    int mute = 2;
//    FILE* outFile = NULL;

    float pitch = 1.0f;
    float speed = 1.0f;

    bool readFrameFinish = true;

    bool isCut = false;
    int end_time = 0;
    bool showPcm = false;

    pthread_t pcmCallBackThread;
    WLBufferQueue *bufferQueue;
    int defaultPcmSize = 4096;

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
    SLMuteSoloItf pcmPlayerMute = NULL;//声道操作

//缓冲器队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

    //SoundTouch
    SoundTouch *soundTouch = NULL;
    SAMPLETYPE *sampleBuffer = NULL;
    bool finished = true;
    uint8_t *out_buffer = NULL;
    int nb = 0;
    int num = 0;

    bool isRecordPcm = false;

    pthread_mutex_t codecMutex;
};

#endif //MYMUSIC_WLAUDIO_H
