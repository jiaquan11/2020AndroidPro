//
// Created by jiaqu on 2020/12/14.
//

#ifndef MYMUSIC_WLBUFFERQUEUE_H
#define MYMUSIC_WLBUFFERQUEUE_H

#include "log/androidLog.h"
#include <deque>
#include "WLPlayStatus.h"
#include "WLPcmBean.h"
#include <pthread.h>

class WLBufferQueue {
public:
    WLBufferQueue(WLPlayStatus *playStatus);

    ~WLBufferQueue();

    int putBuffer(SAMPLETYPE *buffer, int size);

    int getBuffer(WLPcmBean **pcmBean);

    int clearBuffer();

    int getBufferSize();

    void release();

    int noticeThread();

public:
    std::deque<WLPcmBean *> queueBuffer;
    pthread_mutex_t mutexBuffer;
    pthread_cond_t condBuffer;
    WLPlayStatus *wlPlayStatus = NULL;
};

#endif //MYMUSIC_WLBUFFERQUEUE_H
