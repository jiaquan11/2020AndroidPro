#include <jni.h>
#include <string>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <unistd.h>
#include "RecordBuffer.h"
#include "log/androidLog.h"

SLObjectItf slObjectEngine = NULL;
SLEngineItf engineItf = NULL;

SLObjectItf recordObj = NULL;
SLRecordItf recordItf = NULL;

SLAndroidSimpleBufferQueueItf recordBufferQueue = NULL;

RecordBuffer *recordBuffer = NULL;
FILE *pcmFile = NULL;

bool finish = false;

void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
//    LOGI("recordBuffer->getNowBuffer(): %p", recordBuffer->getNowBuffer());
    if (pcmFile != NULL){
        fwrite(recordBuffer->getNowBuffer(), 1, 4096, pcmFile);
    }

    if (finish) {
        LOGI("opengles 录制完成111");
        (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_STOPPED);

        LOGI("opengles 录制完成aaa");
//        (*recordObj)->Destroy(recordObj);//执行此语句会卡住，暂时屏蔽
        recordObj = NULL;
        recordItf = NULL;
        LOGI("opengles 录制完成bbb");
        (*slObjectEngine)->Destroy(slObjectEngine);
        slObjectEngine = NULL;
        engineItf = NULL;
        LOGI("opengles 录制完成ccc");
        delete recordBuffer;
        recordBuffer = NULL;
        LOGI("opengles 录制完成ddd");
        if (pcmFile != NULL) {
            fclose(pcmFile);
            pcmFile = NULL;
        }

        LOGI("opengles 录制完成2222");
    } else {
//        LOGI("recordBuffer->getRecordBuffer(): %p", recordBuffer->getRecordBuffer());
        (*recordBufferQueue)->Enqueue(recordBufferQueue, recordBuffer->getRecordBuffer(), 4096);
        LOGI("opengles 正在录制");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_openslesrecord_MainActivity_startRecord(JNIEnv *env, jobject thiz, jstring jPath) {
    // TODO: implement startRecord()
    if (finish) {
        return;
    }
    const char *path = env->GetStringUTFChars(jPath, 0);

    finish = false;

    pcmFile = fopen(path, "wb");

    recordBuffer = new RecordBuffer(4096);

    slCreateEngine(&slObjectEngine, 0, NULL, 0, NULL, NULL);//创建引擎
    (*slObjectEngine)->Realize(slObjectEngine, SL_BOOLEAN_FALSE);//实例化这个引擎
    (*slObjectEngine)->GetInterface(slObjectEngine, SL_IID_ENGINE, &engineItf);//获取引擎这个集合里面的某个实例对象

    SLDataLocator_IODevice loc_dev = {SL_DATALOCATOR_IODEVICE,
                                      SL_IODEVICE_AUDIOINPUT,
                                      SL_DEFAULTDEVICEID_AUDIOINPUT,
                                      NULL};
    SLDataSource audioSrc = {&loc_dev, NULL};

    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            2};
    SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSink audioSnk = {&loc_bq, &format_pcm};

    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    (*engineItf)->CreateAudioRecorder(engineItf, &recordObj, &audioSrc, &audioSnk, 1, id, req);//引擎集合里面的engineItf对象调用自身接口创建录制器，同时得到一个录制器句柄

    (*recordObj)->Realize(recordObj, SL_BOOLEAN_FALSE);//实例化这个录制器的句柄
    (*recordObj)->GetInterface(recordObj, SL_IID_RECORD, &recordItf);//获取录制器集合里面某个修改录制状态的对象
    (*recordObj)->GetInterface(recordObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &recordBufferQueue);//获取录制器集合里面某个缓冲队列的对象

    (*recordBufferQueue)->Enqueue(recordBufferQueue, recordBuffer->getRecordBuffer(), 4096);//将缓冲buffer入队
    (*recordBufferQueue)->RegisterCallback(recordBufferQueue, bqRecorderCallback, NULL);//注册回调

    (*recordItf)->SetRecordState(recordItf, SL_RECORDSTATE_RECORDING);

    env->ReleaseStringUTFChars(jPath, path);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_openslesrecord_MainActivity_stopRecord(JNIEnv *env, jobject thiz) {
    // TODO: implement stopRecord()
    finish = true;
}