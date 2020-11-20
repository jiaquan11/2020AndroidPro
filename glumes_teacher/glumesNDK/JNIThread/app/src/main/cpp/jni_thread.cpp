//
// Created by jiaqu on 2020/10/31.
//

#include "base.h"
#include <pthread.h>
#include <stdio.h>
#include "jvm.h"
#include <unistd.h>
#include <queue>

struct ThreadRunArgs{
    int id;
    int result;
};

void *printThreadHello(void*){
    LOGI("hello thread");
    //耗时操作
//    return nullptr;
    pthread_exit(0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithread_JNIThread_createNativeThread(JNIEnv* env, jobject instance){
    pthread_t handles;
    int result = pthread_create(&handles, nullptr, printThreadHello, nullptr);
    if (result >= 0){
        LOGI("create thread success");
    }else{
        LOGI("create thread failed!");
    }
}

void *printThreadArgs(void *arg){
    ThreadRunArgs* args = static_cast<ThreadRunArgs *>(arg);
    LOGI("thread id is %d", args->id);
    LOGI("thread result is %d", args->result);
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithread_JNIThread_createNativeThreadWithArgs(JNIEnv* env, jobject instance){
    pthread_t handles;
    ThreadRunArgs* args = new ThreadRunArgs;
    args->id = 2;
    args->result = 100;

    int result = pthread_create(&handles, nullptr, printThreadArgs, args);
    if (result >= 0){
        LOGI("create thread args success result:%d",result);
    }else{
        LOGI("create thread args failed!");
    }
}

void* printThreadJoin(void* arg){
    ThreadRunArgs* args = static_cast<ThreadRunArgs *>(arg);

    struct timeval begin;
    gettimeofday(&begin, nullptr);

    LOGI("start time is %lld", begin.tv_sec);
    sleep(3);
    struct timeval end;
    gettimeofday(&end, nullptr);
    LOGI("end time is %lld", end.tv_sec);
    LOGI("Time used is %d", end.tv_sec - begin.tv_sec);

    return reinterpret_cast<void *>(args->result);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithread_JNIThread_joinNativeThread(JNIEnv* env, jobject instance){
    pthread_t handles;
    ThreadRunArgs* args = new ThreadRunArgs;
    args->id = 2;
    args->result = 50;

    int result = pthread_create(&handles, nullptr, printThreadJoin, args);//返回0表示创建线程成功
    void *ret = nullptr;

    pthread_join(handles, &ret);
    LOGI("result is %d", result);
    LOGI("ret is %d", ret);
}