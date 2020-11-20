//
// Created by jiaqu on 2020/10/27.
//

#ifndef JNICONSTRUCTOR_BASE_H
#define JNICONSTRUCTOR_BASE_H

#include <android/log.h>
#include <jni.h>

#define LOG_TAG "IMOOC_JNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif //JNIREFERENCETYPE_BASE_H
