#include <jni.h>
#include <string>

#include "RtmpPush.h"

RtmpPush* rtmpPush = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_livepusher_push_WLPushVideo_initPush(JNIEnv *env, jobject thiz, jstring push_url) {
    // TODO: implement initPush()
    const char* pushUrl = env->GetStringUTFChars(push_url, 0);

    LOGI("RtmpPush pushUrl: %s", pushUrl);
    rtmpPush = new RtmpPush(pushUrl);
    rtmpPush->init();

    env->ReleaseStringUTFChars(push_url, pushUrl);
}