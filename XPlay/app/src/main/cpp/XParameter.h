//
// Created by jiaqu on 2020/4/6.
//
#ifndef XPLAY_XPARAMETER_H
#define XPLAY_XPARAMETER_H

struct AVCodecParameters;

class XParameter {
public:
    AVCodecParameters* para = 0;
    int channels = 2;
    int sample_rate = 44100;
};

#endif //XPLAY_XPARAMETER_H
