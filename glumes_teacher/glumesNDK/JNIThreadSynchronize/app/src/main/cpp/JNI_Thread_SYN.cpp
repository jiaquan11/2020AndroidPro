//
// Created by jiaqu on 2020/10/31.
//

#include "base.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <queue>

pthread_mutex_t mutex;//互斥变量
pthread_cond_t cond;//条件变量
pthread_t waitHandle;
pthread_t notifyHandle;

int flag = 0;
void* waitThread(void *){
    LOGI("wait thread lock");
    pthread_mutex_lock(&mutex);

    while(flag == 0){
        LOGI("waiting");
        pthread_cond_wait(&cond, &mutex);
    }
    LOGI("wait thread unlock");

    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

void* nofityThread(void *){
    LOGI("notify thread lock");
    pthread_mutex_lock(&mutex);
    flag = 1;
    pthread_mutex_unlock(&mutex);

    pthread_cond_signal(&cond);
    LOGI("singal....");
    LOGI("notify thread unlock");
    pthread_exit(0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithreadsynchronize_JNIThreadSYN_waitNativeThread(
                    JNIEnv* env, jobject instance){
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);

    pthread_create(&waitHandle, nullptr, waitThread, nullptr);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithreadsynchronize_JNIThreadSYN_notifyNativeThread(
        JNIEnv* env, jobject instance){
//    pthread_mutex_init(&mutex, nullptr);
//    pthread_cond_init(&cond, nullptr);

    pthread_create(&notifyHandle, nullptr, nofityThread, nullptr);
}