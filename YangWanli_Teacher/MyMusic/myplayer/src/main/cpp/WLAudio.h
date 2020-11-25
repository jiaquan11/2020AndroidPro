//
// Created by jiaqu on 2020/11/25.
//

#ifndef MYMUSIC_WLAUDIO_H
#define MYMUSIC_WLAUDIO_H

extern "C"{
#include <libavcodec/avcodec.h>
};

class WLAudio {
public:
    WLAudio();
    ~WLAudio();

public:
    int streamIndex = -1;
    AVCodecParameters *codecPar = NULL;
    AVCodecContext *avCodecContext = NULL;
};
#endif //MYMUSIC_WLAUDIO_H
