//
// Created by jiaqu on 2021/2/17.
//

#ifndef LIVEPUSHER_RTMPPUSH_H
#define LIVEPUSHER_RTMPPUSH_H

#include <string.h>
#include <malloc.h>
#include "WLQueue.h"
#include <pthread.h>
#include "CallJava.h"

extern "C" {
#include "librtmp/rtmp.h"
}

class RtmpPush {
public:
    RtmpPush(const char *url, CallJava *callJava);

    ~RtmpPush();

    void init();

    void pushSPSPPS(char* sps, int sps_len, char* pps, int pps_len);
    void pushVideoData(char* data, int data_len, bool keyframe);
    void pushAudioData(char* data, int data_len);
    void pushStop();

public:
    RTMP *rtmp = NULL;
    char *url = NULL;
    WLQueue *queue = NULL;
    pthread_t push_thread;
    CallJava *callJava = NULL;

    bool startPushing = false;
    long startTime = 0;
};
#endif //LIVEPUSHER_RTMPPUSH_H
