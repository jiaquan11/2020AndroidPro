//
// Created by jiaqu on 2020/4/19.
//

#ifndef XPLAY_IPLAYER_H
#define XPLAY_IPLAYER_H

#include <mutex>
#include "XThread.h"
#include "XParameter.h"

class IDemux;

class IAudioPlay;

class IVideoView;

class IResample;

class IDecode;

class IPlayer : public XThread {
public:
    static IPlayer *Get(unsigned char index = 0);

    virtual bool Open(const char *path);

    virtual void Close();

    virtual bool Seek(double pos);

    virtual bool Start();

    virtual void InitView(void *win);

    //获取当前的播放进度 0.0-1.0
    virtual double PlayPos();

    virtual void SetPause(bool isP);

public:
    IDemux *demux = 0;
    IDecode *vdecode = 0;
    IDecode *adecode = 0;
    IResample *resample = 0;
    IVideoView *videoView = 0;
    IAudioPlay *audioPlay = 0;

    //音频输出参数配置
    XParameter outPara;
    //是否视频硬解
    bool isHardDecode = true;

protected:
    IPlayer() {};

    //用作音视频同步
    void Main();

protected:
    std::mutex mux;
};

#endif //XPLAY_IPLAYER_H
