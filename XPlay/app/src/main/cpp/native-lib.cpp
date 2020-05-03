#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "XLog.h"
#include "IPlayerProxy.h"

extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM* vm, void* res){
    IPlayerProxy::Get()->Init(vm);
//    char* url = "/storage/emulated/0/Pictures/v1080.mp4";
//    IPlayerProxy::Get()->Open(url);
//    IPlayerProxy::Get()->Start();
    return JNI_VERSION_1_4;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_xplay_XPlay_InitView(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement InitView()
    ANativeWindow* win = ANativeWindow_fromSurface(env, surface);
    IPlayerProxy::Get()->InitView(win);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_xplay_OpenUrl_Open(JNIEnv *env, jobject thiz, jstring urlStr) {
    // TODO: implement Open()
    const char* url = env->GetStringUTFChars(urlStr, 0);
    XLOGI("XPlay Open");
    IPlayerProxy::Get()->Open(url);
    IPlayerProxy::Get()->Start();

    env->ReleaseStringUTFChars(urlStr, url);
}