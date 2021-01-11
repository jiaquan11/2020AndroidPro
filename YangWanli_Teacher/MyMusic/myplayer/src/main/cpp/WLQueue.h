//
// Created by jiaqu on 2020/11/26.
//

#ifndef MYMUSIC_WLQUEUE_H
#define MYMUSIC_WLQUEUE_H

#include "log/androidLog.h"
#include "queue"
#include "pthread.h"
#include "WLPlayStatus.h"

extern "C" {
#include "libavcodec/avcodec.h"
};

class WLQueue {
public:
    WLQueue(WLPlayStatus *playStatus);

    ~WLQueue();

    int putAVPacket(AVPacket *packet);

    int getAVPacket(AVPacket *packet);

    int getQueueSize();

    void clearAvPacket();

    void noticeQueue();

public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;

    WLPlayStatus *playStatus = NULL;
};

#endif //MYMUSIC_WLQUEUE_H
