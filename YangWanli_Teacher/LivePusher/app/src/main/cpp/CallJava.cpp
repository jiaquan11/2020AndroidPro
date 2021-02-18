//
// Created by jiaqu on 2020/11/25.
//

#include "CallJava.h"

CallJava::CallJava(JavaVM *vm, JNIEnv *env, jobject obj) {
    javaVm = vm;
    jniEnv = env;
    jobj = env->NewGlobalRef(obj);

    jclass clz = jniEnv->GetObjectClass(jobj);
    if (!clz) {
        if (LOG_DEBUG) {
            LOGE("get jclass wrong");
        }
        return;
    }

    jmid_connecting = env->GetMethodID(clz, "onConnecting", "()V");
    jmid_connectsuccess = env->GetMethodID(clz, "onConnectSuccess", "()V");
    jmid_connectfail = env->GetMethodID(clz, "onConnectFail", "(Ljava/lang/String;)V");
}

CallJava::~CallJava() {
    jniEnv->DeleteGlobalRef(jobj);
    javaVm = NULL;
    jniEnv = NULL;
}

void CallJava::onConnecting(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_connecting);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_connecting);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onConnectSuccess(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_connectsuccess);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_connectsuccess);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onConnectFail(int type, char *msg) {
    if (type == MAIN_THREAD) {
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid_connectfail, jmsg);
        jniEnv->DeleteLocalRef(jmsg);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        jstring jmsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jobj, jmid_connectfail, jmsg);
        env->DeleteLocalRef(jmsg);
        javaVm->DetachCurrentThread();
    }
}
