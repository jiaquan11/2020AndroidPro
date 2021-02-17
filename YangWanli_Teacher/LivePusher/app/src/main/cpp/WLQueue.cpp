//
// Created by jiaqu on 2021/2/17.
//

#include "WLQueue.h"

WLQueue::WLQueue() {
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

WLQueue::~WLQueue() {
    clearQueue();

    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int WLQueue::putRtmpPacket(RTMPPacket *packet) {
    pthread_mutex_lock(&mutexPacket);

    queuePacket.push(packet);
    pthread_cond_signal(&condPacket);

    pthread_mutex_unlock(&mutexPacket);

    return 0;
}

RTMPPacket *WLQueue::getRtmpPacket() {
    pthread_mutex_lock(&mutexPacket);
    RTMPPacket *p = NULL;
    if (!queuePacket.empty()){
        p = queuePacket.front();
        queuePacket.pop();
    }else{
        pthread_cond_wait(&condPacket, &mutexPacket);//pthread_cond_wait内部会自动先解锁
    }

    pthread_mutex_unlock(&mutexPacket);
    return p;
}

void WLQueue::clearQueue() {
    pthread_mutex_lock(&mutexPacket);
    while (true){
        if (queuePacket.empty()){
            break;
        }
        RTMPPacket *p = queuePacket.front();
        queuePacket.pop();
        RTMPPacket_Free(p);
        p = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);
}

void WLQueue::notifyQueue() {
    pthread_mutex_lock(&mutexPacket);
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
}
