#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include "FFDemux.h"
#include "XLog.h"
#include "IDecode.h"
#include "FFDecode.h"
#include "XEGL.h"
#include "XShader.h"
#include "IVideoView.h"
#include "GLVideoView.h"
#include "IResample.h"
#include "FFResample.h"
#include "IAudioPlay.h"
#include "SLAudioPlay.h"

IVideoView* view = NULL;

extern "C" JNIEXPORT jstring JNICALL
Java_com_jiaquan_xplay_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    ///////////////////////////////////////////////////////////////////
    //char* url = "/storage/emulated/0/Pictures/biterate9.mp4";
    char* url = "/storage/emulated/0/Pictures/v1080.mp4";
    IDemux *demux = new FFDemux();
    demux->Open(url);

    IDecode* vdecode = new FFDecode();
    vdecode->Open(demux->GetVPara());

    IDecode* adecode = new FFDecode();
    adecode->Open(demux->GetAPara());

    demux->AddObs(vdecode);
    demux->AddObs(adecode);

    view = new GLVideoView();
    vdecode->AddObs(view);

    IResample *resample = new FFResample();
    XParameter outPara = demux->GetAPara();
    resample->Open(demux->GetAPara(), outPara);
    adecode->AddObs(resample);

    IAudioPlay *audioPlay = new SLAudioPlay();
    audioPlay->StartPlay(outPara);
    resample->AddObs(audioPlay);

    demux->Start();
    vdecode->Start();
    adecode->Start();

    //XSleep(3000);
    //demux->Stop();
    //////////////////////////////////////////////////
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_xplay_XPlay_InitView(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement InitView()
    ANativeWindow* win = ANativeWindow_fromSurface(env, surface);
//    XEGL::Get()->Init(win);
//    XShader shader;
//    shader.Init();
      view->SetRender(win);
}