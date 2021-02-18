//
// Created by jiaqu on 2020/11/25.
//

#ifndef MYMUSIC_CALLJAVA_H
#define MYMUSIC_CALLJAVA_H

#include "jni.h"
#include "log/androidLog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class CallJava {
public:
    CallJava(JavaVM *vm, JNIEnv *env, jobject obj);

    ~CallJava();

    void onConnecting(int type);

    void onConnectSuccess(int type);

    void onConnectFail(int type, char *msg);

public:
    JavaVM *javaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj = NULL;

    jmethodID jmid_connecting;
    jmethodID jmid_connectsuccess;
    jmethodID jmid_connectfail;
};

#endif //MYMUSIC_CALLJAVA_H
