#include <jni.h>
#include <string>
#include "FFDemux.h"
#include "XLog.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_jiaquan_xplay_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    ///////////////////////////////////////////////////////////////////
    char* url = "/storage/emulated/0/Pictures/biterate9.mp4";
    IDemux *demux = new FFDemux();
    demux->Open(url);
    demux->Start();

    //////////////////////////////////////////////////
    return env->NewStringUTF(hello.c_str());
}
