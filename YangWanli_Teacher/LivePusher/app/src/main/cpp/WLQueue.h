//
// Created by jiaqu on 2021/2/17.
//

#ifndef LIVEPUSHER_WLQUEUE_H
#define LIVEPUSHER_WLQUEUE_H

#include <queue>
#include <pthread.h>
#include "log/androidLog.h"

extern "C"{
#include "librtmp/rtmp.h"
}

class WLQueue {
public:
    WLQueue();
    ~WLQueue();

    int putRtmpPacket(RTMPPacket* packet);
    RTMPPacket *getRtmpPacket();

    void clearQueue();

    void notifyQueue();

public:
    std::queue<RTMPPacket*> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
};
#endif //LIVEPUSHER_WLQUEUE_H
