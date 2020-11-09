#include "log/androidLog.h"
#include <string>
#include "egl/EglThread.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "GLES2/gl2.h"


ANativeWindow *nativeWindow = NULL;
EglThread* eglThread = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceCreate(JNIEnv *env, jobject thiz, jobject surface) {
    // TODO: implement surfaceCreate()
    nativeWindow = ANativeWindow_fromSurface(env, surface);

    eglThread = new EglThread();
    eglThread->onSurfaceCreate(nativeWindow);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint width,
                                                   jint height) {
    // TODO: implement surfaceChange()
    if (eglThread != NULL){
        eglThread->onSurfaceChange(width, height);
    }
}