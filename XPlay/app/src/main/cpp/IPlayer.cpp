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

void IPlayer::Close() {
    mux.lock();
    //1 先关闭主体线程，再清理观察者
    //同步线程
    XThread:Stop();
    //解封装
    if (demux)
        demux->Stop();
    //解码
    if (vdecode)
        vdecode->Stop();
    if (adecode)
        adecode->Stop();
    //2 清理缓冲队列
    if (vdecode)
        vdecode->Clear();
    if (adecode)
        adecode->Clear();
    if (audioPlay)
        audioPlay->Clear();
    //3清理资源
    if (audioPlay)
        audioPlay->Close();
    if (videoView)
        videoView->Close();
    if (vdecode)
        vdecode->Close();
    if (adecode)
        adecode->Close();
    if (demux)
        demux->Close();
    mux.unlock();
}

//获取当前的播放进度 0.0-1.0
double IPlayer::PlayPos(){
    double pos = 0.0;
    
    mux.lock();
    int totalMs = 0;
    if (demux)
        totalMs = demux->totalMs;
    if (totalMs > 0){
        if (vdecode){
            pos = (double)vdecode->pts/(double)totalMs;
        }
    }
    mux.unlock();
    return pos;
}

bool IPlayer::Open(const char* path){
    Close();

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
    //if (outPara.sample_rate <= 0){
        outPara = demux->GetAPara();
    //}

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

    //需要提前开始音频播放，音视频解码线程，否则demux线程的notify的数据会被
    //丢掉，导致画面绿屏
    if (audioPlay){
        audioPlay->StartPlay(outPara);
    }

    if (adecode){
        adecode->Start();
    }

    if (vdecode){
        vdecode->Start();
    }

    if (!demux || !demux->Start()){
        XLOGE("demux->Start() failed!");
        mux.unlock();
        return false;
    }

    XThread::Start();

    mux.unlock();
    return true;
}

void IPlayer::InitView(void* win){
    if (videoView){
        videoView->Close();
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
        //XLOGI("apts = %d", apts);
        vdecode->synPts = apts;

        mux.unlock();
        XSleep(2);
    }
}