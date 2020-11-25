//
// Created by jiaqu on 2020/11/25.
//

#include "JavaListener.h"

JavaListener::JavaListener(JavaVM *vm, JNIEnv *env, jobject obj) {
    jvm = vm;
    jobj = obj;
    jniEnv = env;

    jclass clz = env->GetObjectClass(jobj);
    if (!clz) {
        return;
    }

    jmethodId = env->GetMethodID(clz, "onError", "(ILjava/lang/String;)V");
}

JavaListener::~JavaListener() {

}

void JavaListener::onError(int threadType, int code, const char *msg) {
    if (threadType == 0) {//子线程
        JNIEnv *env;
        jvm->AttachCurrentThread(&env, 0);
        jstring jmsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jobj, jmethodId, code, jmsg);
        env->DeleteLocalRef(jmsg);

        jvm->DetachCurrentThread();
    } else if (threadType == 1) {//主线程
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmethodId, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);
    }
}
