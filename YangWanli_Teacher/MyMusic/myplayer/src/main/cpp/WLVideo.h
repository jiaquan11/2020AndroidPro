//
// Created by jiaqu on 2020/12/26.
//
#ifndef MYMUSIC_WLVIDEO_H
#define MYMUSIC_WLVIDEO_H

#include "WLQueue.h"
#include "CallJava.h"
#include <pthread.h>

extern "C"{
#include <libswscale/swscale.h>
#include "include/libavcodec/avcodec.h"
#include <libavutil/time.h>
#include <libavutil/imgutils.h>
};

class WLVideo {
public:
    WLVideo(WLPlayStatus* playStatus, CallJava* callJava);
    ~WLVideo();

    void play();

    void release();

public:
    int streamIndex = -1;
    AVCodecContext* avCodecContext = NULL;
    AVCodecParameters *codecPar = NULL;
    WLQueue *queue = NULL;
    WLPlayStatus* playStatus = NULL;
    CallJava* callJava = NULL;
    AVRational time_base;

    pthread_t thread_play;
};
#endif //MYMUSIC_WLVIDEO_H
