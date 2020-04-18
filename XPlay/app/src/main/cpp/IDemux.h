//
// Created by jiaqu on 2020/4/6.
//
#ifndef XPLAY_IDEMUX_H
#define XPLAY_IDEMUX_H

#include "XData.h"
#include "XThread.h"
#include "IObserver.h"
#include "XParameter.h"

//解封装接口
class IDemux : public IObserver{
public:
    //打开文件，或者流媒体rtmp http rtsp
    virtual bool Open(const char* url) = 0;

    //获取视频参数
    virtual XParameter GetVPara() = 0;

    //获取音频参数
    virtual XParameter GetAPara() = 0;

    //读取一帧数据，数据由调用者清理
    virtual XData Read() = 0;

protected:
    virtual void Main();

public:
    //总时长(毫秒)
    int totalMs = 0;
};
#endif //XPLAY_IDEMUX_H
