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
#include "FilterTwo.h"
#include "FilterYUV.h"
#include <string>

class Opengl {
public:
    Opengl();

    ~Opengl();

    void onCreateSurface(JNIEnv *env, jobject surface);

    void onChangeSurface(int width, int height);

    void onChangeSurfaceFilter();

    void onDestroySurface();

    void setPixel(void *data, int width, int height, int length);

    void setYuvData(void* y, void* u, void* v, int w, int h);

public:
    EglThread *eglThread = NULL;
    ANativeWindow *nativeWindow = NULL;
    BaseOpengl *baseOpengl = NULL;

    void *pixels = NULL;

    int pic_width = 0;
    int pic_height = 0;
};


#endif //NATIVEOPENGLDEMO_OPENGL_H
