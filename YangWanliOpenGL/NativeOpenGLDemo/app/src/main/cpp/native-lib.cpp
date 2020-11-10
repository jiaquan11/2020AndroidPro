#include "log/androidLog.h"
#include <string>
#include "egl/EglThread.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "GLES2/gl2.h"

ANativeWindow *nativeWindow = NULL;
EglThread *eglThread = NULL;

void callback_SurfaceCreate(void *ctx) {
    LOGI("callback_SurfaceCreate");
    EglThread *eglThread = static_cast<EglThread *>(ctx);
}

void callback_SurfaceChange(int w, int h, void *ctx) {
    LOGI("callback_SurfaceChange");
    EglThread *eglThread = static_cast<EglThread *>(ctx);
    glViewport(0, 0, w, h);
}

void callback_SurfaceDraw(void *ctx) {
    LOGI("callback_SurfaceDraw");
    EglThread *eglThread = static_cast<EglThread *>(ctx);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);//指定刷屏颜色  1:不透明  0：透明
    glClear(GL_COLOR_BUFFER_BIT);//将刷屏颜色进行刷屏，但此时仍然处于后台缓冲中，需要swapBuffers交换到前台界面显示
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceCreate(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement surfaceCreate()
    nativeWindow = ANativeWindow_fromSurface(env, surface);

    eglThread = new EglThread();
    eglThread->setRenderType(OPENGL_RENDER_HANDLE);
    eglThread->callBackOnCreate(callback_SurfaceCreate, eglThread);
    eglThread->callBackOnChange(callback_SurfaceChange, eglThread);
    eglThread->callBackOnDraw(callback_SurfaceDraw, eglThread);

    eglThread->onSurfaceCreate(nativeWindow);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint width,
                                                   jint height) {
    // TODO: implement surfaceChange()
    if (eglThread != NULL) {
        eglThread->onSurfaceChange(width, height);

        usleep(1000000);
        eglThread->notifyRender();
    }
}