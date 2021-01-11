//
// Created by jiaqu on 2020/12/26.
//
#ifndef MYMUSIC_WLVIDEO_H
#define MYMUSIC_WLVIDEO_H

#include "WLQueue.h"
#include "CallJava.h"
#include <pthread.h>
#include "WLAudio.h"

#define CODEC_YUV 0
#define CODEC_MEDIACODEC 1

extern "C" {
#include <libswscale/swscale.h>
#include "include/libavcodec/avcodec.h"
#include <libavutil/time.h>
#include <libavutil/imgutils.h>
};

class WLVideo {
public:
    WLVideo(WLPlayStatus *playStatus, CallJava *callJava);

    ~WLVideo();

    void play();

    void release();

    double getFrameDiffTime(AVFrame *avFrame, AVPacket* avPacket);

    double getDelayTime(double diff);

public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecPar = NULL;
    WLQueue *queue = NULL;
    WLPlayStatus *playStatus = NULL;
    CallJava *callJava = NULL;
    AVRational time_base;

    pthread_t thread_play;

    WLAudio *audio = NULL;
    double clock = 0;
    double delayTime = 0;
    double defaultDelayTime = 0.04;

    pthread_mutex_t codecMutex;

    int codectype = CODEC_YUV;
    AVBSFContext *abs_ctx = NULL;
};

#endif //MYMUSIC_WLVIDEO_H
