#include <jni.h>
#include <string>

#include "RtmpPush.h"
#include "CallJava.h"

RtmpPush *rtmpPush = NULL;
CallJava *callJava = NULL;
JavaVM *javaVm = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_livepusher_push_WLPushVideo_initPush(JNIEnv *env, jobject thiz, jstring push_url) {
    // TODO: implement initPush()
    const char *pushUrl = env->GetStringUTFChars(push_url, 0);

    LOGI("RtmpPush pushUrl: %s", pushUrl);
    callJava = new CallJava(javaVm, env, thiz);
    rtmpPush = new RtmpPush(pushUrl, callJava);
    rtmpPush->init();

    env->ReleaseStringUTFChars(push_url, pushUrl);
}

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVm = vm;
    JNIEnv *env;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        if (LOG_DEBUG) {
            LOGE("GetEnv failed!");
        }
        return -1;
    }

    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT jint JNICALL JNI_OnUnLoad(JavaVM *vm, void *reserved) {
    javaVm = NULL;
}