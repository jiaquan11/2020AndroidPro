//
// Created by jiaqu on 2020/11/25.
//

#ifndef MYMUSIC_WLAUDIO_H
#define MYMUSIC_WLAUDIO_H

#include "WLQueue.h"
#include "WLPlayStatus.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};

class WLAudio {
public:
    WLAudio(WLPlayStatus* playStatus);
    ~WLAudio();

    void play();

    int resampleAudio();

public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;

    WLQueue* queue = NULL;
    WLPlayStatus* playStatus = NULL;

    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = -1;
    uint8_t *buffer = NULL;
    int data_size = 0;

//    FILE* outFile = NULL;
};
#endif //MYMUSIC_WLAUDIO_H
