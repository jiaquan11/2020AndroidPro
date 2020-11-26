//
// Created by jiaqu on 2020/11/25.
//

#ifndef MYMUSIC_WLAUDIO_H
#define MYMUSIC_WLAUDIO_H

#include "WLQueue.h"
#include "WLPlayStatus.h"

extern "C"{
#include <libavcodec/avcodec.h>
};

class WLAudio {
public:
    WLAudio(WLPlayStatus* playStatus);
    ~WLAudio();

public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;

    WLQueue* queue = NULL;
    WLPlayStatus* playStatus = NULL;
};
#endif //MYMUSIC_WLAUDIO_H
