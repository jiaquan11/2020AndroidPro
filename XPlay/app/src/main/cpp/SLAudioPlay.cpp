//
// Created by jiaqu on 2020/4/19.
//
#include "SLAudioPlay.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "XLog.h"

static SLObjectItf engineSL = NULL;
static SLEngineItf eng = NULL;
static SLObjectItf mix = NULL;
static SLObjectItf  player = NULL;
static SLPlayItf iplayer = NULL;
static SLAndroidSimpleBufferQueueItf pcmQue = NULL;

SLAudioPlay::SLAudioPlay(){
    buf = new unsigned char[1024*1024];
}

SLAudioPlay::~SLAudioPlay(){
    delete []buf;
    buf = 0;
}

static SLEngineItf  CreateSL(){
    SLresult ret;
    SLEngineItf en;
    ret = slCreateEngine(&engineSL, 0, 0, 0, 0, 0);
    if (ret != SL_RESULT_SUCCESS) return NULL;
    ret = (*engineSL)->Realize(engineSL, SL_BOOLEAN_FALSE);
    if (ret != SL_RESULT_SUCCESS) return NULL;
    ret = (*engineSL)->GetInterface(engineSL, SL_IID_ENGINE, &en);
    if (ret != SL_RESULT_SUCCESS) return NULL;
    return en;
}

static void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *context){
    SLAudioPlay* ap = (SLAudioPlay*)context;
    if (!ap){
        XLOGE("PcmCall failed context is null");
        return;
    }
    ap->PlayCall((void*)bf);
}

void SLAudioPlay::PlayCall(void *bufq) {
    if (!bufq)
        return;

    SLAndroidSimpleBufferQueueItf bf = (SLAndroidSimpleBufferQueueItf)bufq;
    //XLOGI("SLAudioPlay PlayCall");

    //阻塞
    XData d = GetData();
    if (d.size <= 0){
        XLOGE("GetData size is 0");
        return;
    }

    if (!buf)
        return;
    memcpy(buf, d.data, d.size);

    mux.lock();
    if (pcmQue && (*pcmQue))
        (*pcmQue)->Enqueue(pcmQue, buf, d.size);
    mux.unlock();
    d.Drop();
}

void SLAudioPlay::Close(){
    IAudioPlay::Clear();

    mux.lock();

    //停止播放
    if (iplayer && (*iplayer)){
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);
    }
    //清理播放队列
    if (pcmQue && (*pcmQue)){
        (*pcmQue)->Clear(pcmQue);
    }
    //销毁player对象
    if (player && (*player)){
        (*player)->Destroy(player);
    }

    //销毁混音器
    if (mix && (*mix)){
        (*mix)->Destroy(mix);
    }

    //销毁播放引擎
    if (engineSL && (*engineSL)){
        (*engineSL)->Destroy(engineSL);
    }

    engineSL = NULL;
    eng = NULL;
    mix = NULL;
    player = NULL;
    iplayer = NULL;
    pcmQue = NULL;

    mux.unlock();
}

bool SLAudioPlay::StartPlay(XParameter out){
    Close();

    mux.lock();

//1 创建引擎
    eng = CreateSL();
    if (eng){
        XLOGI("CreateSL success!");
    }else{
        mux.unlock();
        XLOGE("CreateSL failed!");
        return false;
    }

    //创建混音器
    SLresult ret = 0;
    ret = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (ret != SL_RESULT_SUCCESS){
        mux.unlock();
        XLOGE("CreateOutputMix failed!");
        return false;
    }
    ret = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (ret != SL_RESULT_SUCCESS){
        mux.unlock();
        XLOGE("mix Realize failed!");
        return false;
    }
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outputMix, 0};

    //3.配置音频信息
    //缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    //音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32)out.channels,//声道数
            (SLuint32)out.sample_rate*1000,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN//字节序，小端
    };
    SLDataSource ds = {&que, &pcm};

    //4.创建播放器
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    ret = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink, sizeof(ids)/sizeof(SLInterfaceID), ids, req);
    if (ret != SL_RESULT_SUCCESS){
        mux.unlock();
        XLOGE("CreateAudioPlayer failed!");
        return false;
    }else{
        XLOGI("CreateAudioPlayer success!");
    }

    (*player)->Realize(player, SL_BOOLEAN_FALSE);

    //获取Player接口
    ret = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (ret != SL_RESULT_SUCCESS){
        mux.unlock();
        XLOGE("player GetInterface SL_IID_PLAY failed!");
        return false;
    }else{
        XLOGI("player GetInterface SL_IID_PLAY success!");
    }
    ret = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (ret != SL_RESULT_SUCCESS){
        mux.unlock();
        XLOGE("player GetInterface SL_IID_BUFFERQUEUE failed");
        return false;
    }else{
        XLOGI("player GetInterface SL_IID_BUFFERQUEUE success");
    }

    //设置回调函数，播放队列空调用
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, this);

    //设置播放状态
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);

    //启动队列回调
    (*pcmQue)->Enqueue(pcmQue, "", 1);
    isExit = false;
    XLOGI("SLAudioPlay StartPlay success!");

    mux.unlock();
    return true;
}