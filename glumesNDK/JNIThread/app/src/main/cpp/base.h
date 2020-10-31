//
// Created by jiaqu on 2020/10/31.
//

#ifndef JNITHREAD_BASE_H
#define JNITHREAD_BASE_H

#include <android/log.h>
#include <jni.h>

#define LOG_TAG "IMOOC_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif //JNITHREAD_BASE_H
