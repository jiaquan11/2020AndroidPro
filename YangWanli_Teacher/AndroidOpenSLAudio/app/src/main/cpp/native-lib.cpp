#include <jni.h>
#include <string>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "log/androidLog.h"

// 引擎接口
SLObjectItf engineObject = NULL;
SLEngineItf engineEngine = NULL;

//混音器
SLObjectItf outputMixObject = NULL;
SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;//走廊环境音混响

SLObjectItf pcmPlayerObject = NULL;
SLPlayItf pcmPlayerPlay = NULL;
SLVolumeItf pcmPlayerVolume = NULL;

//缓冲器队列接口
SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

FILE *pcmFile = NULL;
void *buffer = NULL;
uint8_t *out_buffer = NULL;

void getPcmData(void **pcm) {
    while (!feof(pcmFile)) {
        fread(out_buffer, 44100 * 2 * 2, 1, pcmFile);
        if (out_buffer == NULL) {
            LOGE("Read end");
            break;
        } else {
//            LOGI("reading");
        }
        *pcm = out_buffer;
        break;
    }
}

//给到OpenSLES注册的回调函数，会由OpenSLES主动调用，直接将pcm数据放入OpenSLES中的缓冲队列中进行播放
void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    getPcmData(&buffer);
    if (buffer != NULL) {
        SLresult result;
        result = (*pcmBufferQueue)->Enqueue(pcmBufferQueue, buffer, 44100 * 2 * 2);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_androidopenslaudio_MainActivity_playPcm(JNIEnv *env, jobject thiz,
                                                         jstring urlStr) {
    // TODO: implement playPcm()
    const char *url = env->GetStringUTFChars(urlStr, 0);

    pcmFile = fopen(url, "r");
    if (pcmFile == NULL) {
        return;
    }

    out_buffer = (uint8_t *) malloc(44100 * 2 * 2);//分配了1s的音频数据内存

    SLresult result;

    //1.创建接口对象(根据engineObject接口类对象来创建引擎对象,后面的操作都根据这个引擎对象创建相应的操作接口)
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    //2.设置混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};//设置使能混响音效
    const SLboolean mreq[1] = {SL_BOOLEAN_TRUE};

    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids,
                                              mreq);//通过引擎创建混音器
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);//实现混音器实例
    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,//指定混响音效类型
                                              &outputMixEnvironmentalReverb);//通过混音器设备得到混响音效的实例
    if (SL_RESULT_SUCCESS == result) {
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb,
                &reverbSettings);//设置某种指定的混响音效，比如走廊混响
        (void) result;
    }

    //3.创建播放器(录音器)
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};//指定了两个buffer队列
    SLDataFormat_PCM pcm = {//指定设备进行播放的pcm格式参数，按照指定的参数设置进行播放
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSink = {&outputMix, NULL};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
                                  SL_IID_VOLUME};//指定使能缓存队列和音量操作的接口
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    //根据引擎创建音频播放器实例
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource,
                                                &audioSink, 3,
                                                ids, req);
    // 初始化播放器
    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    //得到接口后调用  获取Player接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY,
                                     &pcmPlayerPlay);//根据音频播放器实例获取到音频播放接口

    //4.设置缓存队列和回调函数
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE,
                                     &pcmBufferQueue);//根据音频播放器实例获取到音频缓存队列的接口
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, NULL);

    //获取音量接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmPlayerVolume);

    //5.设置播放状态
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);

    //6.启动回调函数
    pcmBufferCallBack(pcmBufferQueue, NULL);

    env->ReleaseStringUTFChars(urlStr, url);
}