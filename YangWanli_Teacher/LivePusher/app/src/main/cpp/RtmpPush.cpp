//
// Created by jiaqu on 2021/2/17.
//

#include "RtmpPush.h"

RtmpPush::RtmpPush(const char *url) {
    this->url = static_cast<char *>(malloc(512));
    strcpy(this->url, url);

    this->queue = new WLQueue();
}

RtmpPush::~RtmpPush() {
    queue->notifyQueue();

    queue->clearQueue();

    free(url);
    url = NULL;
}

void *callBackPush(void *data) {
    LOGI("RtmpPush callBackPush in");
    RtmpPush *rtmpPush = static_cast<RtmpPush *>(data);

    rtmpPush->rtmp = RTMP_Alloc();
    RTMP_Init(rtmpPush->rtmp);
    rtmpPush->rtmp->Link.timeout = 10;//10s
    rtmpPush->rtmp->Link.lFlags |= RTMP_LF_LIVE;

    LOGI("RtmpPush callBackPush in 111");

    RTMP_SetupURL(rtmpPush->rtmp, rtmpPush->url);
    RTMP_EnableWrite(rtmpPush->rtmp);
    LOGI("RtmpPush callBackPush in 222");

    if (!RTMP_Connect(rtmpPush->rtmp, NULL)){
        LOGE("RtmpPush can not connect the url");
        goto end;
    }

    LOGI("RtmpPush callBackPush in 333");

    if (!RTMP_ConnectStream(rtmpPush->rtmp, 0)){
        LOGE("RtmpPush can not connect the stream of service");
        goto end;
    }

    LOGI("RtmpPush: rtmp连接成功，开始推流！");

//    while (true){
//
//    }
    end:
        RTMP_Close(rtmpPush->rtmp);
        RTMP_Free(rtmpPush->rtmp);
        rtmpPush->rtmp = NULL;

    LOGI("RtmpPush: 推流结束！");
    pthread_exit(&rtmpPush->push_thread);
}

void RtmpPush::init() {
    pthread_create(&push_thread, NULL, callBackPush, this);
}
