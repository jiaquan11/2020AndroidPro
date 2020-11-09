//
// Created by jiaqu on 2020/11/8.
//

#ifndef NATIVEOPENGLDEMO_EGLTHREAD_H
#define NATIVEOPENGLDEMO_EGLTHREAD_H

#include <EGL/eglplatform.h>
#include "pthread.h"
#include "android/native_window.h"
#include "EglHelper.h"
#include <unistd.h>
#include <GLES2/gl2.h>

class EglThread {
public:
    EglThread();
    ~EglThread();

public:
    void onSurfaceCreate(EGLNativeWindowType window);
    void onSurfaceChange(int width, int height);

public:
    pthread_t pEglThread = -1;
    ANativeWindow *nativeWindow = NULL;

    bool isCreate = false;
    bool isChange = false;
    bool isExit = false;
    bool isStart = false;

    int surfaceWidth = 0;
    int surfaceHeight = 0;
};


#endif //NATIVEOPENGLDEMO_EGLTHREAD_H
