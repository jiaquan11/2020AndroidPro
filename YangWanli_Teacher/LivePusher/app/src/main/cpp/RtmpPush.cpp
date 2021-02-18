//
// Created by jiaqu on 2021/2/17.
//

#include "RtmpPush.h"

RtmpPush::RtmpPush(const char *url, CallJava *callJava) {
    this->url = static_cast<char *>(malloc(512));
    strcpy(this->url, url);

    this->callJava = callJava;
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

    rtmpPush->callJava->onConnecting(CHILD_THREAD);

    rtmpPush->rtmp = RTMP_Alloc();
    RTMP_Init(rtmpPush->rtmp);
    rtmpPush->rtmp->Link.timeout = 10;//10s
    rtmpPush->rtmp->Link.lFlags |= RTMP_LF_LIVE;

    RTMP_SetupURL(rtmpPush->rtmp, rtmpPush->url);
    RTMP_EnableWrite(rtmpPush->rtmp);

    if (!RTMP_Connect(rtmpPush->rtmp, NULL)) {
//        LOGE("RtmpPush can not connect the url");
        rtmpPush->callJava->onConnectFail(CHILD_THREAD, "RtmpPush can not connect the url");
        goto end;
    }

    if (!RTMP_ConnectStream(rtmpPush->rtmp, 0)) {
//        LOGE("RtmpPush can not connect the stream of service");
        rtmpPush->callJava->onConnectFail(CHILD_THREAD,
                                          "RtmpPush can not connect the stream of service");
        goto end;
    }

    rtmpPush->callJava->onConnectSuccess(CHILD_THREAD);

//    LOGI("RtmpPush: rtmp连接成功，开始推流！");

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
