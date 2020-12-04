//
// Created by jiaqu on 2020/11/25.
//

#ifndef MYMUSIC_WLFFMPEG_H
#define MYMUSIC_WLFFMPEG_H

#include "CallJava.h"
#include "pthread.h"
#include "WLAudio.h"

extern "C"{
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};

class WLFFmpeg {
public:
    WLFFmpeg(WLPlayStatus *playStatus, CallJava* calljava, const char* url);
    ~WLFFmpeg();

    void prepared();
    void start();
    void pause();
    void resume();

    void release();

    void decodeFFmpegThread();

public:
    CallJava* callJava = NULL;
    char url[256] = {0};

    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    WLAudio *pWLAudio = NULL;

    WLPlayStatus *playStatus = NULL;

    pthread_mutex_t init_mutex;
    bool isExit = false;
};
#endif //MYMUSIC_WLFFMPEG_H
