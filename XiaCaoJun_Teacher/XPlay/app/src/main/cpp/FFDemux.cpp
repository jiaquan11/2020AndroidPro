//
// Created by jiaqu on 2020/4/6.
//
#include "FFDemux.h"
#include "XLog.h"

extern "C" {
#include "libavformat/avformat.h"
}

//分数转为浮点数
static double r2d(AVRational r) {
    return ((r.num == 0) || (r.den == 0)) ? 0. : (double) r.num / (double) r.den;
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (isFirst) {
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

void FFDemux::Close() {
    mux.lock();
    if (ic) {
        avformat_close_input(&ic);
    }
    mux.unlock();
}

//seek 位置(pos: 0.0-1.0)
bool FFDemux::Seek(double pos) {
    if (pos < 0 || pos > 1) {
        XLOGE("Seek value must 0.0-1.0");
        return false;
    }

    bool ret = false;
    mux.lock();
    if (!ic) {
        mux.unlock();
        return false;
    }
    //清除读取的缓冲
    avformat_flush(ic);

    long long seekPts = 0;
    seekPts = ic->streams[videoStream]->duration * pos;

    //往后跳转到关键帧
    ret = av_seek_frame(ic, videoStream, seekPts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);

    mux.unlock();
    return ret;
}

//打开文件，或者流媒体rtmp http rtsp
bool FFDemux::Open(const char *url) {
    XLOGI("Open file %s begin", url);

    Close();

    mux.lock();
    int ret = avformat_open_input(&ic, url, 0, 0);
    if (ret != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        XLOGE("FFDemux avformat_open_input %s failed!, reason: %s", url, buf);
        return false;
    }
    XLOGI("FFDemux open %s success!", url);

    //读取文件信息
    ret = avformat_find_stream_info(ic, 0);
    if (ret != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        XLOGE("FFDemux avformat_find_stream_info failed!");
        return false;
    }

    this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
    XLOGI("total ms = %d", totalMs);

    mux.unlock();

    GetVPara();
    GetAPara();
    return true;
}

//获取视频参数
XParameter FFDemux::GetVPara() {
    mux.lock();

    if (!ic) {
        mux.unlock();
        XLOGE("GetVPara failed! ic is NULL!");
        return XParameter();
    }
    //获取视频流索引
    int ret = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (ret < 0) {
        mux.unlock();
        XLOGE("av_find_best_stream AVMEDIA_TYPE_VIDEO failed!");
        return XParameter();
    }

    videoStream = ret;

    XParameter para;
    para.para = ic->streams[ret]->codecpar;
    mux.unlock();
    return para;
}

//获取音频参数
XParameter FFDemux::GetAPara() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        XLOGE("GetVPara failed! ic is NULL!");
        return XParameter();
    }
    //获取音频流索引
    int ret = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (ret < 0) {
        mux.unlock();
        XLOGE("av_find_best_stream AVMEDIA_TYPE_AUDIO failed!");
        return XParameter();
    }

    audioStream = ret;

    XParameter para;
    para.para = ic->streams[ret]->codecpar;
    para.channels = ic->streams[ret]->codecpar->channels;
    para.sample_rate = ic->streams[ret]->codecpar->sample_rate;
    mux.unlock();
    return para;
}

//读取一帧数据，数据由调用者清理
XData FFDemux::Read() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        return XData();
    }

    XData d;
    AVPacket *pkt = av_packet_alloc();
    int ret = av_read_frame(ic, pkt);
    if (ret != 0) {
        mux.unlock();
        av_packet_free(&pkt);
        return XData();
    }
    //XLOGI("packet size is %d, pts is %lld", pkt->size, pkt->pts);
    d.data = (unsigned char *) pkt;
    d.size = pkt->size;
    if (pkt->stream_index == audioStream) {
        d.isAudio = true;
    } else if (pkt->stream_index == videoStream) {
        d.isAudio = false;
    } else {
        mux.unlock();
        av_packet_free(&pkt);
        return XData();
    }

    //转换pts  转换为实际时间戳，去掉时间基
    pkt->pts = pkt->pts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    d.pts = (int) pkt->pts;

    if (!d.isAudio){
        XLOGI("demux packet pts %lld, dts: %lld", pkt->pts, pkt->dts);
    }
    //XLOGI("demux pts %d", d.pts);
    mux.unlock();
    return d;
}