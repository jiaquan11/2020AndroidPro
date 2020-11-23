#include <jni.h>
#include <string>

#include "log/androidLog.h"

//extern "C"{
//#include "include/libavformat/avformat.h"
//#include "include/libavcodec/avcodec.h"
//#include "include/libavutil/avutil.h"
//}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_jiaquan_myplayer_Demo_stringFromJNI(JNIEnv *env, jobject thiz) {
    // TODO: implement stringFromJNI()
    LOGI("Java_com_jiaquan_myplayer_Demo_stringFromJNI");
    return env->NewStringUTF("I am xia jia quan ooooo");
}

//extern "C"
//JNIEXPORT void JNICALL
//Java_com_jiaquan_myplayer_Demo_testFFmpeg(JNIEnv *env, jobject thiz) {
//    // TODO: implement testFFmpeg()
//    av_register_all();
//    AVCodec* c_temp = av_codec_next(NULL);
//    while (c_temp != NULL){
//        switch (c_temp->type) {
//            case AVMEDIA_TYPE_VIDEO:
//                LOGI("[video]: %s", c_temp->name);
//                break;
//            case AVMEDIA_TYPE_AUDIO:
//                LOGI("[audio]: %s", c_temp->name);
//                break;
//            default:
//                LOGI("[other]: %s", c_temp->name);
//                break;
//        }
//        c_temp = c_temp->next;
//    }
//}