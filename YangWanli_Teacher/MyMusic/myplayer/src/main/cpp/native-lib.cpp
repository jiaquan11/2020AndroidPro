#include <jni.h>
#include <string>
#include "log/androidLog.h"
#include "CallJava.h"
#include "WLFFmpeg.h"

extern "C" {
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/avutil.h"
}

JavaVM *javaVM = NULL;
CallJava *callJava = NULL;
WLFFmpeg *fFmpeg = NULL;
WLPlayStatus *playStatus = NULL;

bool isExit = true;

pthread_t thread_start;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_myplayer_player_WLPlayer__1prepared(JNIEnv *env, jobject thiz, jstring sourceStr) {
    // TODO: implement _prepared()
    LOGI("call jni prepared!");
    const char *source = env->GetStringUTFChars(sourceStr, 0);
    if (fFmpeg == NULL) {
        if (callJava == NULL) {
            callJava = new CallJava(javaVM, env, thiz);
        }

        callJava->onCallLoad(MAIN_THREAD, true);

        playStatus = new WLPlayStatus();
        fFmpeg = new WLFFmpeg(playStatus, callJava, source);
        fFmpeg->prepared();
    }
    env->ReleaseStringUTFChars(sourceStr, source);
}

void *startCallBack(void* data){
    WLFFmpeg* wlfFmpeg = (WLFFmpeg *)(data);
    wlfFmpeg->start();

    pthread_exit(&thread_start);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_myplayer_player_WLPlayer__1start(JNIEnv *env, jobject thiz) {
    // TODO: implement _start()
    if (fFmpeg != NULL) {
//        fFmpeg->start();
        pthread_create(&thread_start, NULL, startCallBack, fFmpeg);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_myplayer_player_WLPlayer__1pause(JNIEnv *env, jobject thiz) {
    // TODO: implement _pause()
    if (fFmpeg != NULL){
        fFmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_myplayer_player_WLPlayer__1resume(JNIEnv *env, jobject thiz) {
    // TODO: implement _resume()
    if (fFmpeg != NULL){
        fFmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_myplayer_player_WLPlayer__1stop(JNIEnv *env, jobject thiz) {
    // TODO: implement _stop()

    if (!isExit){
        return;
    }
    isExit = false;

    if (fFmpeg != NULL){
        fFmpeg->release();
        delete fFmpeg;
        fFmpeg = NULL;

        if (callJava != NULL){
            delete callJava;
            callJava = NULL;
        }

        if (playStatus != NULL){
            delete playStatus;
            playStatus = NULL;
        }
    }

    isExit = true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_myplayer_player_WLPlayer__1seek(JNIEnv *env, jobject thiz, jint secds) {
    // TODO: implement _seek()
    if (fFmpeg != NULL){
        fFmpeg->seek(secds);
    }
}