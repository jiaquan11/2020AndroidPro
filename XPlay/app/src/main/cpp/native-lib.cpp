#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "XLog.h"
#include "IPlayerProxy.h"

extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM* vm, void* res){
    IPlayerProxy::Get()->Init(vm);
    char* url = "/storage/emulated/0/Pictures/v1080.mp4";
    IPlayerProxy::Get()->Open(url);
    IPlayerProxy::Get()->Start();
    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_jiaquan_xplay_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */, jstring urlStr) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_xplay_XPlay_InitView(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement InitView()
    ANativeWindow* win = ANativeWindow_fromSurface(env, surface);
    IPlayerProxy::Get()->InitView(win);
}