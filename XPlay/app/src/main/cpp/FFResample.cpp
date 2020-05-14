//
// Created by jiaqu on 2020/4/18.
//
extern "C"{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#include "FFResample.h"
#include "XLog.h"

void FFResample::Close(){
    mux.lock();
    if (actx){
        swr_free(&actx);
    }
    mux.unlock();
}

bool FFResample::Open(XParameter in, XParameter out){
    Close();

    mux.lock();
    //音频重采样上下文初始化
    actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(out.channels),
                              AV_SAMPLE_FMT_S16, out.sample_rate,//输出
                              av_get_default_channel_layout(in.para->channels),
                              (AVSampleFormat)in.para->format, in.para->sample_rate,//输入
                              0, 0);
    int ret = swr_init(actx);
    if (ret != 0){
        mux.unlock();
        XLOGE("swr_init failed!");
    }else{
        XLOGI("swr_init success!");
    }

    outChannels = in.para->channels;
    outFormat = AV_SAMPLE_FMT_S16;

    mux.unlock();
    return true;
}

XData FFResample::Resample(XData indata){
    if ((indata.size <= 0) || !indata.data)
        return XData();

    mux.lock();

    if (!actx){
        mux.unlock();
        return XData();
    }

    AVFrame* frame = (AVFrame*)indata.data;

    //输出空间的分配
    XData out;
    int outSize = outChannels*frame->nb_samples*av_get_bytes_per_sample((AVSampleFormat)outFormat);
    if (outSize <= 0){
        mux.unlock();
        return XData();
    }

    out.Alloc(outSize);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.data;
    int len = swr_convert(actx, outArr, frame->nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
    if (len <= 0){
        mux.unlock();
        out.Drop();
        return XData();
    }

    out.pts = indata.pts;

    mux.unlock();
    return out;
}