//
// Created by jiaqu on 2020/11/8.
//

#include "EglThread.h"

EglThread::EglThread() {

}

EglThread::~EglThread() {

}

void* eglThreadImpl(void* context){
    EglThread* eglThread = static_cast<EglThread *>(context);
    if (eglThread != NULL){
        EglHelper* eglHelper = new EglHelper();
        eglHelper->initEgl(eglThread->nativeWindow);
        eglThread->isExit = false;
        while (true){
            if (eglThread->isCreate){
                LOGI("eglThread call surface create!");
                eglThread->isCreate = false;
            }

            if (eglThread->isChange){
                LOGI("eglThread call surface change!");
                eglThread->isChange = false;
                //opengl
                glViewport(0, 0, eglThread->surfaceWidth, eglThread->surfaceHeight);//指定显示窗口大小
                eglThread->isStart = true;
            }

            //绘制
            LOGI("draw");
            if (eglThread->isStart){
                glClearColor(0.0f, 1.0f, 1.0f, 1.0f);//指定刷屏颜色
                glClear(GL_COLOR_BUFFER_BIT);//将刷屏颜色进行刷屏，但此时仍然处于后台缓冲中，需要swapBuffers交换到前台界面显示
                eglHelper->swapBuffers();
            }

            usleep(1000000/60);//六十分之一秒  每秒60次渲染

            if (eglThread->isExit){
                break;
            }
        }
    }

    pthread_exit(&eglThread->pEglThread);
    return 0;
}

void EglThread::onSurfaceCreate(EGLNativeWindowType window) {
    if (pEglThread == -1){
        isCreate = true;
        nativeWindow = window;

        //创建子线程，在子线程中进行EGL的初始化及渲染
        pthread_create(&pEglThread, NULL, eglThreadImpl, this);
    }
}

void EglThread::onSurfaceChange(int width, int height) {
    isChange = true;
    surfaceWidth = width;
    surfaceHeight = height;
}
