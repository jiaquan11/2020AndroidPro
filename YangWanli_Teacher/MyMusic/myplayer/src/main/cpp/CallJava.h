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

    void onCallPrepared(int type);

    void onCallLoad(int type, bool load);

    void onCallTimeInfo(int type, int curr, int total);

    void onCallError(int type, int code, char *msg);

    void onCallComplete(int type);

    void onCallVolumeDB(int type, int db);

    void onCallPcmToAAC(int type, void *buffer, int size);

    void onCallPcmInfo(int type, void *buffer, int size);

    void onCallPcmRate(int type, int samplerate, int bit, int channels);

    void onCallRenderYUV(int type, int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);

public:
    JavaVM *javaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj = NULL;

    jmethodID jmid_prepared;
    jmethodID jmid_load;
    jmethodID jmid_timeinfo;
    jmethodID jmid_error;
    jmethodID jmid_complete;
    jmethodID jmid_volumeDB;
    jmethodID jmid_pcmtoaac;
    jmethodID jmid_pcminfo;
    jmethodID jmid_pcmrate;
    jmethodID jmid_renderyuv;
};
#endif //MYMUSIC_CALLJAVA_H
