//
// Created by jiaqu on 2020/4/6.
//
#include "FFDemux.h"
#include "XLog.h"
extern "C"{
#include "libavformat/avformat.h"
}

FFDemux::FFDemux(){
    static bool isFirst = true;
    if (isFirst){
        //注册所有封装器
        av_register_all();
        //注册所有的解码器
        avcodec_register_all();
        //初始化网络
        avformat_network_init();
        isFirst = false;
        XLOGI("register ffmpeg");
    }
}

//打开文件，或者流媒体rtmp http rtsp
bool FFDemux::Open(const char* url){
    XLOGI("Open file %s begin", url);
    int ret = avformat_open_input(&ic, url, 0, 0);
    if (ret != 0){
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        XLOGE("FFDemux avformat_open_input %s failed!", url);
        return false;
    }
    XLOGI("FFDemux open %s success!", url);

    //读取文件信息
    ret = avformat_find_stream_info(ic, 0);
    if (ret != 0){
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        XLOGE("FFDemux avformat_find_stream_info failed!");
        return false;
    }

    this->totalMs = ic->duration/(AV_TIME_BASE/1000);
    XLOGI("total ms = %d", totalMs);

    return true;
}

//读取一帧数据，数据由调用者清理
XData FFDemux::Read(){
    if (!ic)
        return XData();
    XData d;
    AVPacket* pkt = av_packet_alloc();
    int ret = av_read_frame(ic, pkt);
    if (ret != 0){
        av_packet_free(&pkt);
        return XData();
    }
    XLOGI("packet size is %d, pts is %lld", pkt->size, pkt->pts);
    d.data = (unsigned char*)pkt;
    d.size = pkt->size;
    return d;
}