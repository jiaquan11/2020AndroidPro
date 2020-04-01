#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "testff"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavcodec/jni.h>
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

    //初始化解封装
    av_register_all();
    //初始化网络
    avformat_network_init();
    avcodec_register_all();

    //打开文件
    AVFormatContext* ic = NULL;
    char path[] = "/sdcard/testziliao/biterate9.mp4";
    int ret = avformat_open_input(&ic, path, 0, 0);
    if (ret != 0){
        LOGE("avformat_open_input failed!: %s", av_err2str(ret));
        return env->NewStringUTF(hello.c_str());
    }

    LOGI("avformat_open_input %s success", path);

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
        return env->NewStringUTF(hello.c_str());
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
        return env->NewStringUTF(hello.c_str());
    }else{
        LOGI("avcodec_open2 video success!");
    }
////////////////////////////////////////////////////////////////////////////////
    //音频解码器
    AVCodec* acodec = avcodec_find_decoder(ic->streams[audioStream]->codecpar->codec_id);
    if (!acodec){
        LOGE("avcodec audio find failed!");
        return env->NewStringUTF(hello.c_str());
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
        return env->NewStringUTF(hello.c_str());
    }else{
        LOGI("avcodec_open2 audio success!");
    }

    //读取帧数据
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    long long start = GetNowMs();
    int frameCount = 0;
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

        //LOGI("stream=%d size=%d pts=%lld flag=%d", pkt->stream_index, pkt->size, pkt->pts, pkt->flags);

        //只测试视频
//        if (pkt->stream_index != videoStream){
//            continue;
//        }
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
            }
        }
    }
    //关闭上下文
    avformat_close_input(&ic);
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