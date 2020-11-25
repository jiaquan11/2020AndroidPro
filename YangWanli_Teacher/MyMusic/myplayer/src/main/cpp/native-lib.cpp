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
        fFmpeg = new WLFFmpeg(callJava, source);
        fFmpeg->prepared();
    }
    env->ReleaseStringUTFChars(sourceStr, source);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_myplayer_player_WLPlayer__1start(JNIEnv *env, jobject thiz) {
    // TODO: implement _start()
    if (fFmpeg != NULL) {
        fFmpeg->start();
    }
}