//
// Created by jiaqu on 2020/12/14.
//

#include "WLBufferQueue.h"

WLBufferQueue::WLBufferQueue(WLPlayStatus *playStatus) {
    wlPlayStatus = playStatus;
    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_cond_init(&condBuffer, NULL);
}

WLBufferQueue::~WLBufferQueue() {
    wlPlayStatus = NULL;
    pthread_mutex_destroy(&mutexBuffer);
    pthread_cond_destroy(&condBuffer);
    if (LOG_DEBUG) {
        LOGI("WLBufferQueue 释放完成");
    }
}

int WLBufferQueue::putBuffer(SAMPLETYPE *buffer, int size) {
    pthread_mutex_lock(&mutexBuffer);
//    LOGI("WLBufferQueue putBuffer size: %d", size);
    WLPcmBean *pcmBean = new WLPcmBean(buffer, size);
    queueBuffer.push_back(pcmBean);
    pthread_cond_signal(&condBuffer);
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int WLBufferQueue::getBuffer(WLPcmBean **pcmBean) {
    pthread_mutex_lock(&mutexBuffer);
    while ((wlPlayStatus != NULL) && !wlPlayStatus->isExit) {
        if (queueBuffer.size() > 0) {
            *pcmBean = queueBuffer.front();
            queueBuffer.pop_front();
            break;
        } else {
            if (!wlPlayStatus->isExit) {
                pthread_cond_wait(&condBuffer, &mutexBuffer);
            }
        }
    }

    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int WLBufferQueue::clearBuffer() {
    pthread_cond_signal(&condBuffer);
    pthread_mutex_lock(&mutexBuffer);
    while (!queueBuffer.empty()) {
        WLPcmBean *pcmBean = queueBuffer.front();
        queueBuffer.pop_front();
        delete pcmBean;
    }
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

void WLBufferQueue::release() {
    if (LOG_DEBUG) {
        LOGI("WLBufferQueue::release()");
    }
    noticeThread();
    clearBuffer();
    if (LOG_DEBUG) {
        LOGI("WLBufferQueue::release() success!");
    }
}

int WLBufferQueue::getBufferSize() {
    int size = 0;
    pthread_mutex_lock(&mutexBuffer);
    size = queueBuffer.size();
    pthread_mutex_unlock(&mutexBuffer);
    return size;
}

int WLBufferQueue::noticeThread() {
    pthread_cond_signal(&condBuffer);
    return 0;
}
