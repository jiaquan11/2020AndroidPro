//
// Created by jiaqu on 2020/11/1.
//
#include "base.h"
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <queue>
using namespace std;

queue<int> data;
pthread_mutex_t dataMutex;
pthread_cond_t dataCond;

void *productThread(void *){
    while (data.size() < 10){
        pthread_mutex_lock(&dataMutex);
        LOGI("生产物品");
        data.push(1);
        if (data.size() > 0){
            LOGI("等待消费");
            pthread_cond_signal(&dataCond);
        }
        pthread_mutex_unlock(&dataMutex);
        sleep(3);
    }

    pthread_exit(0);
}

void *consumerThread(void *){
    while (1){
        pthread_mutex_lock(&dataMutex);
        if (data.size() > 0){
            LOGI("消费物品");
            data.pop();
        }else{
            LOGI("等待生产");
            pthread_cond_wait(&dataCond, &dataMutex);
        }
        pthread_mutex_unlock(&dataMutex);
        sleep(1);
    }
    pthread_exit(0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jniproduceconsumer_JNIProduceConsumer_startProductAndConsumer(
        JNIEnv* env, jobject instance) {
    pthread_mutex_init(&dataMutex, nullptr);
    pthread_cond_init(&dataCond, nullptr);

    pthread_t productHandle;
    pthread_t consumerHandle;

    pthread_create(&productHandle, nullptr, productThread, nullptr);
    pthread_create(&consumerHandle, nullptr, consumerThread, nullptr);
}



