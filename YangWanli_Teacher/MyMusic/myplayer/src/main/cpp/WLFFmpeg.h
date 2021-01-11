//
// Created by jiaqu on 2020/11/25.
//

#ifndef MYMUSIC_WLFFMPEG_H
#define MYMUSIC_WLFFMPEG_H

#include "CallJava.h"
#include "pthread.h"
#include "WLAudio.h"
#include "WLVideo.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};

class WLFFmpeg {
public:
    WLFFmpeg(WLPlayStatus *playStatus, CallJava *calljava, const char *url);

    ~WLFFmpeg();

    void prepared();

    void start();

    void pause();

    void resume();

    void seek(int64_t secds);

    void release();

    int getCodecContext(AVCodecParameters *codecPar, AVCodecContext** avCodecContext);

    void setVolume(int percent);

    void setMute(int mute);

    void setPitch(float pitch);

    void setSpeed(float speed);

    int getSampleRate();

    void startStopRecord(bool start);

    bool cutAudioPlay(int start_time, int end_time, bool showPcm);

    void decodeFFmpegThread();

public:
    CallJava *callJava = NULL;
    char url[256] = {0};

    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    WLAudio *pWLAudio = NULL;
    WLVideo* pWLVideo = NULL;

    WLPlayStatus *playStatus = NULL;

    pthread_mutex_t init_mutex;
    bool isExit = false;

    int duration = 0;
    pthread_mutex_t seek_mutex;

    bool supportMediaCodec = false;

    const AVBitStreamFilter * bsFilter = NULL;
};

#endif //MYMUSIC_WLFFMPEG_H
