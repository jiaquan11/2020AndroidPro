//
// Created by jiaqu on 2020/11/26.
//

#include "WLQueue.h"

WLQueue::WLQueue(WLPlayStatus *playStatus) {
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
    this->playStatus = playStatus;
}

WLQueue::~WLQueue() {
    clearAvPacket();

    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int WLQueue::putAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    queuePacket.push(packet);
    if (LOG_DEBUG) {
//        LOGI("put a packet into queue, the count: %d", queuePacket.size());
    }

    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int WLQueue::getAVPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    while ((playStatus != NULL) && !playStatus->isExit) {
        if (queuePacket.size() > 0) {
            AVPacket *avPacket = queuePacket.front();
            /*
             * av_packet_ref将原packet的buf引用赋值给目标packet,建立一个新的引用
             * 其它字段全部进行拷贝
             * */
            if (av_packet_ref(packet, avPacket) == 0) {
                queuePacket.pop();
            }
            /*
             * av_packet_free
             * avPacket如果有引用计数，av_packet_free会取消引用，但不会释放数据buf。
             * 只有没有引用计数为0，才会释放数据buf
             * 释放packet malloc的内存
             * */
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            if (LOG_DEBUG) {
//                LOGI("get a packet from the queue, the rest: %d", queuePacket.size());
            }
            break;
        } else {
            pthread_cond_wait(&condPacket, &mutexPacket);
        }
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int WLQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);
    return size;
}

void WLQueue::clearAvPacket() {
    pthread_cond_signal(&condPacket);

    pthread_mutex_lock(&mutexPacket);
    while (!queuePacket.empty()) {
        AVPacket *packet = queuePacket.front();
        queuePacket.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);
}

void WLQueue::noticeQueue() {
    pthread_cond_signal(&condPacket);
}
