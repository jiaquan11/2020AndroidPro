#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#define LOG_TAG "testff"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavcodec/jni.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

static double r2d(AVRational r){
    return (r.num == 0 || r.den == 0) ? 0 : (double)r.num/(double)r.den;
}

//当前时间戳clock
long long GetNowMs(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int sec = tv.tv_sec%360000;//只取100小时内，防止溢出
    long long t = sec*1000+tv.tv_usec/1000;
    return t;
}

extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM* vm, void* res){
    av_jni_set_java_vm(vm, 0);
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    hello += avcodec_configuration();
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_testffmpeg_MainActivity_Open(JNIEnv *env, jobject thiz, jstring urlStr,
                                              jobject handle) {
    // TODO: implement Open()
    const char* url = env->GetStringUTFChars(urlStr, 0);
    FILE* fp = fopen(url, "rb");
    if (!fp){
        LOGE("File %s open failed!", url);
    }else{
        LOGI("File %s open success!", url);
        fclose(fp);
    }

    env->ReleaseStringUTFChars(urlStr, url);

    return true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_testffmpeg_XPlay_Open(JNIEnv *env, jobject thiz, jstring urlStr, jobject surface) {
    // TODO: implement Open()
    const char* url = env->GetStringUTFChars(urlStr, 0);

    //初始化解封装
    av_register_all();
    //初始化网络
    avformat_network_init();
    avcodec_register_all();

    //打开文件
    AVFormatContext* ic = NULL;
    //char path[] = "/sdcard/testziliao/biterate9.mp4";
    int ret = avformat_open_input(&ic, url, 0, 0);
    if (ret != 0){
        LOGE("avformat_open_input failed!: %s", av_err2str(ret));
        return;
    }

    LOGI("avformat_open_input %s success", url);

    //获取流信息
    ret = avformat_find_stream_info(ic, 0);
    if (ret != 0){
        LOGE("avformat_find_stream_info failed");
    }
    LOGI("duration= %lld, nb_streams = %d", ic->duration, ic->nb_streams);

    int fps = 0;
    int width = 0;
    int height = 0;
    int videoStream = 0;
    int audioStream = 0;

    for (int i = 0; i < ic->nb_streams; ++i) {
        AVStream* as = ic->streams[i];
        if (as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            LOGI("视频数据");
            videoStream = i;
            fps = r2d(as->avg_frame_rate);
            LOGI("fps= %d, width=%d height=%d codecId=%d", fps,
                 as->codecpar->width,
                 as->codecpar->height,
                 as->codecpar->codec_id);

        }else if (as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO){
            LOGI("音频数据");
            audioStream = i;
            LOGI("sample_rate: %d channels: %d sample_format: %d",
                 as->codecpar->sample_rate,
                 as->codecpar->channels,
                 as->codecpar->format);
        }
    }

    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    LOGI("av_find_best_stream audioStream: %d", audioStream);

    ////////////////////////////////////////////////////////
    //视频解码器
    //软解码器
    AVCodec* vcodec = avcodec_find_decoder(ic->streams[videoStream]->codecpar->codec_id);
    //硬解码
    //vcodec = avcodec_find_decoder_by_name("h264_mediacodec");
    if (!vcodec){
        LOGE("avcodec video find failed!");
        return;
    }
    else{
        LOGI("avcodec video find success!");
    }
    //解码器初始化
    AVCodecContext* vc = avcodec_alloc_context3(vcodec);
    avcodec_parameters_to_context(vc, ic->streams[videoStream]->codecpar);
    vc->thread_count = 8;
    //打开视频解码器
    ret = avcodec_open2(vc, 0, 0);
    if (ret != 0){
        LOGE("avcodec_open2 video failed!");
        return;
    }else{
        LOGI("avcodec_open2 video success!");
    }
////////////////////////////////////////////////////////////////////////////////
    //音频解码器
    AVCodec* acodec = avcodec_find_decoder(ic->streams[audioStream]->codecpar->codec_id);
    if (!acodec){
        LOGE("avcodec audio find failed!");
        return;
    }else{
        LOGI("avcodec audio find success!");
    }
    //解码器初始化
    AVCodecContext* ac = avcodec_alloc_context3(acodec);
    avcodec_parameters_to_context(ac, ic->streams[audioStream]->codecpar);
    ac->thread_count = 8;
    //打开视频解码器
    ret = avcodec_open2(ac, 0, 0);
    if (ret != 0){
        LOGE("avcodec_open2 audio failed!");
        return;
    }else{
        LOGI("avcodec_open2 audio success!");
    }

    //读取帧数据
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    long long start = GetNowMs();
    int frameCount = 0;

    //初始化像素格式转换的上下文
    SwsContext* vctx = NULL;
    int outWidth = 1280;
    int outHeight = 720;
    char* rgb = new char[1920*1080*4];

    //音频重采样上下文初始化
    SwrContext *actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(2),
                              AV_SAMPLE_FMT_S16, ac->sample_rate,
                              av_get_default_channel_layout(ac->channels),
                              ac->sample_fmt, ac->sample_rate,
                              0, 0);
    ret = swr_init(actx);
    if (ret != 0){
        LOGE("swr_init failed!");
    }else{
        LOGI("swr_init success!");
    }
    char* pcm = new char[48000*4*2];

    //显示窗口初始化
    ANativeWindow* nwin = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(nwin, outWidth, outHeight,WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer wbuf;

    for (;;) {
        //超过3秒
        if (GetNowMs() - start >= 3000){
            LOGI("now decode fps is %d", frameCount/3);
            start = GetNowMs();
            frameCount = 0;
        }

        int ret = av_read_frame(ic, pkt);
        if (ret != 0){
            LOGI("读取到结尾处");
            int ms = 1000;
            long long pos = (double)ms / (double)1000 / r2d(ic->streams[pkt->stream_index]->time_base);
            av_seek_frame(ic, videoStream, pos, AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);
            continue;
        }

        AVCodecContext* cc = vc;
        if (pkt->stream_index == audioStream)
            cc = ac;

        //发送到线程中解码
        ret = avcodec_send_packet(cc, pkt);
        //清理
        av_packet_unref(pkt);
        if (ret != 0){
            LOGE("avcodec_send_packet failed!");
            continue;
        }

        for(;;){
            ret = avcodec_receive_frame(cc, frame);
            if (ret != 0){
                //LOGE("avcodec_receive_frame failed!");
                break;
            }
            //LOGI("avcodec_receive_frame %lld", frame->pts);
            //如果是视频帧
            if (cc == vc){
                frameCount++;
                vctx = sws_getCachedContext(vctx,
                                            frame->width,
                                            frame->height,
                                            (AVPixelFormat)frame->format,
                                            outWidth,
                                            outHeight,
                                            AV_PIX_FMT_RGBA,
                                            SWS_FAST_BILINEAR,
                                            0, 0, 0);
                if (!vctx){
                    LOGE("sws_getCachedContext failed!");
                }else{
                    uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
                    data[0] = (uint8_t*)rgb;
                    int lines[AV_NUM_DATA_POINTERS] = {0};
                    lines[0] = outWidth*4;
                    int h = sws_scale(vctx, (const uint8_t**)frame->data, frame->linesize, 0, frame->height,
                                      data, lines);
                    LOGI("sws_scale = %d", h);
                    if (h > 0){
                        ANativeWindow_lock(nwin, &wbuf, 0);
                        uint8_t *dst = (uint8_t*)wbuf.bits;
                        memcpy(dst, rgb, outWidth*outHeight*4);
                        ANativeWindow_unlockAndPost(nwin);
                    }
                }
            }else{//音频
                uint8_t *out[2] = {0};
                out[0] = (uint8_t*)pcm;
                //音频重采样
                int len = swr_convert(actx, out,
                                      frame->nb_samples,
                                      (const uint8_t**)frame->data,
                                      frame->nb_samples);
                LOGI("swr_convert len= %d", len);
            }
        }
    }

    delete []rgb;
    //关闭上下文
    avformat_close_input(&ic);

    env->ReleaseStringUTFChars(urlStr, url);
}