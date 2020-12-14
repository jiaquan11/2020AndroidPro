//
// Created by jiaqu on 2020/12/14.
//

#ifndef MYMUSIC_WLPCMBEAN_H
#define MYMUSIC_WLPCMBEAN_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "soundtouch/include/SoundTouch.h"

using namespace soundtouch;

class WLPcmBean {
public:
    WLPcmBean(SAMPLETYPE *buffer, int size);

    ~WLPcmBean();

public:
    char *buffer;
    int buffsize;
};

#endif //MYMUSIC_WLPCMBEAN_H
