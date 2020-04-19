//
// Created by jiaqu on 2020/4/19.
//
#ifndef XPLAY_SLAUDIOPLAY_H
#define XPLAY_SLAUDIOPLAY_H

#include "IAudioPlay.h"

class SLAudioPlay : public IAudioPlay{
public:
    SLAudioPlay();
    virtual ~SLAudioPlay();

    virtual bool StartPlay(XParameter out);
    void PlayCall(void *bufq);

protected:
    unsigned char* buf = 0;
};
#endif //XPLAY_SLAUDIOPLAY_H
