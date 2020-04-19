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
    XLOGE("Open file %s begin", url);
    int ret = avformat_open_input(&ic, url, 0, 0);
    if (ret != 0){
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        XLOGE("FFDemux avformat_open_input %s failed!, reason: %s", url, buf);
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

    GetVPara();
    GetAPara();

    return true;
}

//获取视频参数
XParameter FFDemux::GetVPara(){
    if (!ic){
        XLOGE("GetVPara failed! ic is NULL!");
        return XParameter();
    }
    //获取视频流索引
    int ret = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (ret < 0){
        XLOGE("av_find_best_stream AVMEDIA_TYPE_VIDEO failed!");
        return XParameter();
    }

    videoStream = ret;

    XParameter para;
    para.para = ic->streams[ret]->codecpar;
    return para;
}

//获取音频参数
XParameter FFDemux::GetAPara(){
    if (!ic){
        XLOGE("GetVPara failed! ic is NULL!");
        return XParameter();
    }
    //获取音频流索引
    int ret = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (ret < 0){
        XLOGE("av_find_best_stream AVMEDIA_TYPE_AUDIO failed!");
        return XParameter();
    }

    audioStream = ret;

    XParameter para;
    para.para = ic->streams[ret]->codecpar;
    para.channels = ic->streams[ret]->codecpar->channels;
    para.sample_rate = ic->streams[ret]->codecpar->sample_rate;
    return para;
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
    //XLOGI("packet size is %d, pts is %lld", pkt->size, pkt->pts);
    d.data = (unsigned char*)pkt;
    d.size = pkt->size;
    if (pkt->stream_index == audioStream){
        d.isAudio = true;
    }else if (pkt->stream_index == videoStream){
        d.isAudio = false;
    }else{
        av_packet_free(&pkt);
        return XData();
    }

    return d;
}