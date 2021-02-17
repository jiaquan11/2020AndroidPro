//
// Created by jiaqu on 2021/2/17.
//

#ifndef LIVEPUSHER_RTMPPUSH_H
#define LIVEPUSHER_RTMPPUSH_H

#include <string.h>
#include <malloc.h>
#include "WLQueue.h"
#include <pthread.h>

extern "C"{
#include "librtmp/rtmp.h"
}

class RtmpPush {
public:
    RtmpPush(const char* url);
    ~RtmpPush();

    void init();

public:
    RTMP* rtmp = NULL;
    char* url = NULL;
    WLQueue* queue = NULL;
    pthread_t push_thread;
};
#endif //LIVEPUSHER_RTMPPUSH_H
