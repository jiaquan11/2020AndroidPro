//
// Created by jiaqu on 2020/11/25.
//

#ifndef JNITHREAD_JAVALISTENER_H
#define JNITHREAD_JAVALISTENER_H

#include "jni.h"

class JavaListener {
public:
    JavaListener(JavaVM* vm, JNIEnv *env, jobject obj);
    ~JavaListener();

    /**
     * 1:主线程
     * 0：子线程
     * @param threadType
     * @param code
     * @param msg
     */
    void onError(int threadType, int code, const char* msg);

public:
    JavaVM* jvm;
    jobject jobj;
    JNIEnv* jniEnv;

    jmethodID jmethodId;
};
#endif //JNITHREAD_JAVALISTENER_H
