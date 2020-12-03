//
// Created by jiaqu on 2020/11/25.
//

#include "CallJava.h"

CallJava::CallJava(JavaVM *vm, JNIEnv *env, jobject obj) {
    javaVm = vm;
    jniEnv = env;
    jobj = env->NewGlobalRef(obj);

    jclass clz = jniEnv->GetObjectClass(jobj);
    if (!clz){
        if (LOG_DEBUG){
            LOGE("get jclass wrong");
        }
        return;
    }

    jmid_prepared = env->GetMethodID(clz, "onCallPrepared", "()V");
    jmid_load = env->GetMethodID(clz, "onCallLoad", "(Z)V");
    jmid_timeinfo = env->GetMethodID(clz, "onCallTimeInfo", "(II)V");
}

CallJava::~CallJava() {

}

void CallJava::onCallPrepared(int type) {
    if (type == MAIN_THREAD){
        jniEnv->CallVoidMethod(jobj, jmid_prepared);
    }else if (type == CHILD_THREAD){
        JNIEnv* env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_prepared);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallLoad(int type, bool load) {
    if (type == MAIN_THREAD){
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
    }else if (type == CHILD_THREAD){
        JNIEnv* env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_load, load);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallTimeInfo(int type, int curr, int total) {
    if (type == MAIN_THREAD){
        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
    }else if (type == CHILD_THREAD){
        JNIEnv* env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK){
            if (LOG_DEBUG){
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
        javaVm->DetachCurrentThread();
    }
}
