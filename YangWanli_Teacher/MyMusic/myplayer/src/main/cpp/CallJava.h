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
    CallJava(JavaVM* vm, JNIEnv* env, jobject obj);
    ~CallJava();

    void onCallPrepared(int type);

public:
    JavaVM* javaVm = NULL;
    JNIEnv* jniEnv = NULL;
    jobject jobj = NULL;

    jmethodID jmid_prepared;
};


#endif //MYMUSIC_CALLJAVA_H
