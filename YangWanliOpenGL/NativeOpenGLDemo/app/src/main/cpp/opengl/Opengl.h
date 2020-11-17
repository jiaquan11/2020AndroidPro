//
// Created by jiaqu on 2020/11/15.
//

#ifndef NATIVEOPENGLDEMO_OPENGL_H
#define NATIVEOPENGLDEMO_OPENGL_H

#include "../egl/EglThread.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "BaseOpengl.h"
#include "FilterOne.h"
#include <string>

class Opengl {
public:
    Opengl();

    ~Opengl();

    void onCreateSurface(JNIEnv *env, jobject surface);

    void onChangeSurface(int width, int height);

    void onDestroySurface();

    void setPixel(void *data, int width, int height, int length);

public:
    EglThread *eglThread = NULL;
    ANativeWindow *nativeWindow = NULL;
    BaseOpengl *baseOpengl = NULL;

    void *pixels = NULL;
};


#endif //NATIVEOPENGLDEMO_OPENGL_H
