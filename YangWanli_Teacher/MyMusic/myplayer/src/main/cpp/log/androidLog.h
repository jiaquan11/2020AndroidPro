//
// Created by jiaqu on 2020/11/8.
//

#ifndef NATIVEOPENGLDEMO_ANDROIDLOG_H
#define NATIVEOPENGLDEMO_ANDROIDLOG_H

#include <android/log.h>
#include <jni.h>

#define LOG_DEBUG true

#define LOG_TAG "MYPLAYER"
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, FORMAT, ##__VA_ARGS__)
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, FORMAT, ##__VA_ARGS__)
#define LOGW(FORMAT, ...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, FORMAT, ##__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, FORMAT, ##__VA_ARGS__)

#endif //NATIVEOPENGLDEMO_ANDROIDLOG_H
