//
// Created by jiaqu on 2020/4/6.
//
#ifndef XPLAY_FFDEMUX_H
#define XPLAY_FFDEMUX_H

#include "IDemux.h"

struct AVFormatContext;

class FFDemux : public IDemux{
public:
    FFDemux();

public:
    //打开文件，或者流媒体rtmp http rtsp
    virtual bool Open(const char* url);

    //读取一帧数据，数据由调用者清理
    virtual XData Read();

private:
    AVFormatContext* ic = 0;
};

#endif //XPLAY_FFDEMUX_H
