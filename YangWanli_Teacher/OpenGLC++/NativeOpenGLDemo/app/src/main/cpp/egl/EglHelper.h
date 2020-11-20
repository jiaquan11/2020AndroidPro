//
// Created by jiaqu on 2020/11/8.
//

#ifndef NATIVEOPENGLDEMO_EGLHELPER_H
#define NATIVEOPENGLDEMO_EGLHELPER_H

#include "EGL/egl.h"
#include "../log/androidLog.h"

class EglHelper {
public:
    EglHelper();

    ~EglHelper();

public:
    int initEgl(EGLNativeWindowType window);

    int swapBuffers();

    void destroyEgl();

public:
    EGLDisplay mEglDisplay;
    EGLSurface mEglSurface;
    EGLConfig mEglConfig;
    EGLContext mEglContext;
};

#endif //NATIVEOPENGLDEMO_EGLHELPER_H
