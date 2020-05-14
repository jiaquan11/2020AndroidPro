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

IPlayer *IPlayer::Get(unsigned char index) {//单例模式
    static IPlayer p[256];
    return &p[index];
}

void IPlayer::Close() {
    mux.lock();
    //1 先关闭主体线程，再清理观察者
    //同步线程
    XThread:
    Stop();
    //解封装
    if (demux)
        demux->Stop();
    //解码
    if (vdecode)
        vdecode->Stop();
    if (adecode)
        adecode->Stop();
    if (audioPlay)
        audioPlay->Stop();

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

void IPlayer::SetPause(bool isP) {
    mux.lock();
    XThread::SetPause(isP);
    if (demux)
        demux->SetPause(isP);
    if (vdecode)
        vdecode->SetPause(isP);
    if (adecode)
        adecode->SetPause(isP);
    if (audioPlay)
        audioPlay->SetPause(isP);
    mux.unlock();
}

//获取当前的播放进度 0.0-1.0
double IPlayer::PlayPos() {
    double pos = 0.0;
    mux.lock();
    int totalMs = 0;
    if (demux)
        totalMs = demux->totalMs;
    if (totalMs > 0) {
        if (vdecode) {
            pos = (double) vdecode->pts / (double) totalMs;
        }
    }
    mux.unlock();
    return pos;
}

bool IPlayer::Seek(double pos) {
    bool ret = false;

    if (!demux)
        return false;

    //暂停所有的线程
    SetPause(true);
    mux.lock();
    //清理缓冲
    if (vdecode)
        vdecode->Clear();//清理缓冲队列，清理ffmpeg的缓冲
    if (adecode)
        adecode->Clear();
    if (audioPlay)
        audioPlay->Clear();

    ret = demux->Seek(pos);//seek到关键帧
    if (!vdecode) {
        mux.unlock();
        SetPause(false);
        return ret;
    }
    //解码到实际需要显示的帧
    int seekPts = pos * demux->totalMs;
    while (!isExit) {
        XData pkt = demux->Read();
        if (pkt.size <= 0) {
            break;
        }
        if (pkt.isAudio) {
            if (pkt.pts < seekPts) {//对于音频，直接丢掉之前的包即可
                pkt.Drop();
                continue;
            }
            //写入缓冲队列
            demux->Notify(pkt);
            continue;
        }

        //对于视频需要解码，得到解码输出的时间戳，再丢掉解码帧，不然直接丢掉视频包会导致花屏
        //解码需要显示的帧之前的数据
        vdecode->SendPacket(pkt);
        pkt.Drop();
        XData data = vdecode->RecvFrame();
        if (data.size <= 0) {
            continue;
        }
        if (data.pts >= seekPts) {
            //vdecode->Notify(data);
            break;
        }
    }

    mux.unlock();
    SetPause(false);
    return ret;
}

bool IPlayer::Open(const char *path) {
    Close();

    mux.lock();
    //解封装
    if (!demux || !demux->Open(path)) {
        XLOGE("demux->Open %s failed!", path);
        mux.unlock();
        return false;
    }

    //解码，解码可能不需要，如果是解封之后就是原始数据
    if (!vdecode || !vdecode->Open(demux->GetVPara(), isHardDecode)) {
        XLOGE("vdecode->Open failed!");
        //return false;
    }

    if (!adecode || !adecode->Open(demux->GetAPara())) {
        XLOGE("adecode->Open failed!");
        //return false;
    }

    //重采样，有可能不需要，解码后或者解封后可能是直接能播放的数据
    //if (outPara.sample_rate <= 0){
    outPara = demux->GetAPara();
    //}

    if (!resample || !resample->Open(demux->GetAPara(), outPara)) {
        XLOGE("resample->Open failed!");
        //return false;
    }

    XLOGI("IPlayer Open success!");
    mux.unlock();
    return true;
}

bool IPlayer::Start() {
    mux.lock();

    //需要提前开始音频播放，音视频解码线程，否则demux线程的notify的数据会被
    //丢掉，导致画面绿屏
    if (audioPlay) {
        audioPlay->StartPlay(outPara);
    }

    if (adecode) {
        adecode->Start();
    }

    if (vdecode) {
        vdecode->Start();
    }

    if (!demux || !demux->Start()) {
        XLOGE("demux->Start() failed!");
        mux.unlock();
        return false;
    }

    //这个线程用于音视频同步控制
    XThread::Start();

    mux.unlock();
    return true;
}

void IPlayer::InitView(void *win) {
    if (videoView) {
        videoView->Close();
        videoView->SetRender(win);
    }
}

void IPlayer::Main() {
    while (!isExit) {
        mux.lock();
        if (!audioPlay || !vdecode) {
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