//
// Created by jiaqu on 2020/4/19.
//
#include "IPlayer.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IAudioPlay.h"
#include "IVideoView.h"
#include "IResample.h"
#include "XLog.h"

IPlayer* IPlayer::Get(unsigned char index){
    static IPlayer p[256];
    return &p[index];
}

bool IPlayer::Open(const char* path){
    mux.lock();
    //解封装
    if (!demux || !demux->Open(path)){
        XLOGE("demux->Open %s failed!", path);
        mux.unlock();
        return false;
    }

    //解码，解码可能不需要，如果是解封之后就是原始数据
    if (!vdecode || !vdecode->Open(demux->GetVPara(), isHardDecode)){
        XLOGE("vdecode->Open failed!");
        //return false;
    }

    if (!adecode || !adecode->Open(demux->GetAPara())){
        XLOGE("adecode->Open failed!");
        //return false;
    }

    //重采样，有可能不需要，解码后或者解封后可能是直接能播放的数据
    if (outPara.sample_rate <= 0){
        outPara = demux->GetAPara();
    }

    if (!resample || !resample->Open(demux->GetAPara(), outPara)){
        XLOGE("resample->Open failed!");
        //return false;
    }

    XLOGI("IPlayer Open success!");
    mux.unlock();
    return true;
}

bool IPlayer::Start(){
    mux.lock();
    if (!demux || !demux->Start()){
        XLOGE("demux->Start() failed!");
        mux.unlock();
        return false;
    }
    if (adecode){
        adecode->Start();
    }
    if (audioPlay){
        audioPlay->StartPlay(outPara);
    }
    if (vdecode){
        vdecode->Start();
    }

    XThread::Start();

    mux.unlock();
    return true;
}

void IPlayer::InitView(void* win){
    if (videoView){
        videoView->SetRender(win);
    }
}

void IPlayer::Main(){
    while (!isExit){
        mux.lock();
        if (!audioPlay || !vdecode){
            mux.unlock();
            XSleep(2);
            continue;
        }

        //同步
        //获取音频的pts 告诉视频
        int apts = audioPlay->pts;
        XLOGI("apts = %d", apts);
        vdecode->synPts = apts;

        mux.unlock();
        XSleep(2);
    }
}