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
JNIEXPORT void JNICALL
Java_com_jiaquan_livepusher_push_WLPushVideo_pushSPSPPS(JNIEnv *env, jobject thiz, jbyteArray spsArray,
                                                        jint sps_len, jbyteArray ppsArray,
                                                        jint pps_len) {
    // TODO: implement pushSPSPPS()
    jbyte *sps = env->GetByteArrayElements(spsArray, NULL);
    jbyte *pps = env->GetByteArrayElements(ppsArray, NULL);
    if (rtmpPush != NULL){
        rtmpPush->pushSPSPPS(reinterpret_cast<char *>(sps), sps_len, reinterpret_cast<char *>(pps), pps_len);
    }
    env->ReleaseByteArrayElements(spsArray, sps, 0);
    env->ReleaseByteArrayElements(ppsArray, pps, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_livepusher_push_WLPushVideo_pushVideoData(JNIEnv *env, jobject thiz,
                                                           jbyteArray dataArray, jint data_len, jboolean keyframe) {
    // TODO: implement pushVideoData()
    jbyte *data = env->GetByteArrayElements(dataArray, NULL);
    if (rtmpPush != NULL){
        rtmpPush->pushVideoData(reinterpret_cast<char *>(data), data_len, keyframe);
    }
    env->ReleaseByteArrayElements(dataArray, data, 0);
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