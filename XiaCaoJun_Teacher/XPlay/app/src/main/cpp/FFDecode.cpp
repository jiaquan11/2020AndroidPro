//
// Created by jiaqu on 2020/4/6.
//
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>
}

#include "XLog.h"
#include "FFDecode.h"

void FFDecode::InitHard(void *vm) {
    av_jni_set_java_vm(vm, 0);
}

void FFDecode::Clear() {
    IDecode::Clear();
    mux.lock();
    if (codec) {
        avcodec_flush_buffers(codec);//刷新解码器缓冲
    }
    mux.unlock();
}

void FFDecode::Close() {
    IDecode::Clear();

    mux.lock();
    pts = 0;
    if (frame) {
        av_frame_free(&frame);
    }
    if (codec) {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }

    if (datas[0] != 0){
        delete datas[0];
        datas[0] = 0;
    }
    if (datas[1] != 0){
        delete datas[1];
        datas[1] = 0;
    }
    if (datas[2] != 0){
        delete datas[2];
        datas[2] = 0;
    }

    mux.unlock();
}

//打开解码器
bool FFDecode::Open(XParameter para, bool isHard) {
    Close();

    if (!para.para) return false;

    AVCodecParameters *p = para.para;
    //1.查找解码器
    AVCodec *cd = avcodec_find_decoder(p->codec_id);
    if (isHard) {
        cd = avcodec_find_decoder_by_name("h264_mediacodec");
    }

    if (!cd) {
        XLOGE("avcodec_find_decoder %d failed, isHard: %d", p->codec_id, isHard);
        return false;
    }
    XLOGI("avcodec_find_decoder %d, success! isHard: %d", p->codec_id, isHard);

    mux.lock();

    //2 创建解码上下文，并复制参数
    codec = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context(codec, p);
    codec->thread_count = 8;

    //3.打开解码器
    int ret = avcodec_open2(codec, 0, 0);
    if (ret != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        XLOGE("%s", buf);
        return false;
    }

    if (codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        this->isAudio = false;
    } else {
        this->isAudio = true;
    }
    XLOGI("avcodec_open2 success!");

    if (datas[0] == 0){
        datas[0] = new unsigned char[para.para->width*para.para->height];//Y
    }
    if (datas[1] == 0){
        datas[1] = new unsigned char[para.para->width*para.para->height / 4];//U
    }
    if (datas[2] == 0){
        datas[2] = new unsigned char[para.para->width*para.para->height / 4];//V
    }

    mux.unlock();
    return true;
}

//future模型 发送数据到线程解码
bool FFDecode::SendPacket(XData pkt) {
    if ((pkt.size <= 0) || !pkt.data) return false;

    mux.lock();

    if (!codec) {
        mux.unlock();
        return false;
    }
    int ret = avcodec_send_packet(codec, (AVPacket *) pkt.data);
    mux.unlock();
    if (ret != 0) {
        return false;
    }
    return true;
}

//从线程中获取解码结果
XData FFDecode::RecvFrame() {
    mux.lock();
    if (!codec) {
        mux.unlock();
        return XData();
    }
    if (!frame) {
        frame = av_frame_alloc();
    }

    int ret = avcodec_receive_frame(codec, frame);
    if (ret != 0) {
        mux.unlock();
        return XData();
    }

    XData d;
    d.data = (unsigned char *) frame;
    if (codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        d.size = (frame->linesize[0] + frame->linesize[1] + frame->linesize[2]) * frame->height;
        d.width = frame->width;//frame->linesize[0];
        d.height = frame->height;
        XLOGI("linesize: %d %d %d height: %d", frame->linesize[0], frame->linesize[1], frame->linesize[2], frame->height);
    } else {
        //样本字节数*单通道样本数*通道数
        d.size = av_get_bytes_per_sample((AVSampleFormat) frame->format) * frame->nb_samples * 2;
    }

    d.format = frame->format;

    XLOGI("isAudio: %d, data format is %d", isAudio, frame->format);

    if (d.width == frame->linesize[0]) {//无需对齐
        memcpy(d.datas, frame->data, sizeof(d.datas));
    }else {//行对齐
        for (int i = 0; i < d.height; i++) {//Y
            memcpy(datas[0] + d.width * i, frame->data[0] + frame->linesize[0] * i, d.width);
        }
        for (int i = 0; i < d.height/2; i++) {//U
            memcpy(datas[1] + d.width/2 * i, frame->data[1] + frame->linesize[1] * i, d.width/2);
        }
        for (int i = 0; i < d.height/2; i++) {//V
            memcpy(datas[2] + d.width/2 * i, frame->data[2] + frame->linesize[2] * i, d.width/2);
        }

        memcpy(d.datas, datas, sizeof(d.datas));
    }

    d.pts = frame->pts;
    pts = d.pts;//解码时间戳
    if (!isAudio){
        XLOGI("FFDecode video frame pts is %lld", frame->pts);
    }
    mux.unlock();
    return d;
}