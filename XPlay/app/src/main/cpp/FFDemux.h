//
// Created by jiaqu on 2020/4/6.
//
#ifndef XPLAY_FFDEMUX_H
#define XPLAY_FFDEMUX_H

#include "IDemux.h"
#include <mutex>

struct AVFormatContext;

class FFDemux : public IDemux{
public:
    FFDemux();

public:
    //打开文件，或者流媒体rtmp http rtsp
    virtual bool Open(const char* url);

    virtual void Close();

    //seek 位置(pos: 0.0-1.0)
    virtual bool Seek(double pos);

    //获取视频参数
    virtual XParameter GetVPara();

    //获取音频参数
    virtual XParameter GetAPara();

    //读取一帧数据，数据由调用者清理
    virtual XData Read();

private:
    AVFormatContext* ic = 0;
    int audioStream = 1;
    int videoStream = 0;
    std::mutex mux;
};

#endif //XPLAY_FFDEMUX_H
