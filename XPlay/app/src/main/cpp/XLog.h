//
// Created by jiaqu on 2020/4/6.
//
#ifndef XPLAY_XLOG_H
#define XPLAY_XLOG_H

class XLog {

};

#ifdef ANDROID
#include <android/log.h>
#define LOG_TAG "XPlay"
#define  XLOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  XLOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#define  XLOGI(...)  printf("XPlay", __VA_ARGS__)
#define  XLOGE(...)  printf("XPlay", __VA_ARGS__)
#endif

#endif //XPLAY_XLOG_H
