#include "log/androidLog.h"
#include <string>
#include "egl/EglThread.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "GLES2/gl2.h"
#include "shaderutil/shaderUtil.h"

ANativeWindow *nativeWindow = NULL;
EglThread *eglThread = NULL;

//#define GET_STR(x) #x
//const char* vertex = GET_STR(
//        attribute vec2 a_position;
//        void main(){
//            gl_Position = a_position;
//        });
//
//const char* fragment = GET_STR(
//        precision mediump float;
//        void main() {
//            gl_FragColor = vec4(1.0f, 0f, 0f, 1.0f);
//        });


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

    LOGI("before onSurfaceCreate");
    eglThread->onSurfaceCreate(nativeWindow);
    LOGI("after onSurfaceCreate");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint width,
                                                   jint height) {
    // TODO: implement surfaceChange()
    if (eglThread != NULL) {
        LOGI("before onSurfaceChange");
        eglThread->onSurfaceChange(width, height);
        LOGI("after onSurfaceChange");

//        LOGI("before usleep");
//        usleep(5000000);//10s
//        LOGI("after usleep");
        eglThread->notifyRender();
        LOGI("after notifyRender");
    }
}