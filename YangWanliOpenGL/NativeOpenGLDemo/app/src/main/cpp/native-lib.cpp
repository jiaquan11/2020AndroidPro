#include "log/androidLog.h"
#include <string>
#include "opengl/Opengl.h"

Opengl *opengl = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceCreate(JNIEnv *env, jobject thiz, jobject surface) {
    if (opengl == NULL) {
        opengl = new Opengl();
    }
    opengl->onCreateSurface(env, surface);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceChange(JNIEnv *env, jobject thiz, jint width,
                                                   jint height) {
    if (opengl != NULL) {
        opengl->onChangeSurface(width, height);//传入屏幕宽高
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_surfaceDestroy(JNIEnv *env, jobject thiz) {
    // TODO: implement surfaceDestroy()
    if (opengl != NULL) {
        opengl->onDestroySurface();
        delete opengl;
        opengl = NULL;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_imgData(JNIEnv *env, jobject thiz, jint width, jint height,
                                             jint length,
                                             jbyteArray dataArray) {//传入解码后的图像rgba数据
    // TODO: implement imgData()
    jbyte *data = env->GetByteArrayElements(dataArray, NULL);
    if (opengl != NULL) {
        opengl->setPixel(data, width, height, length);
    }
    env->ReleaseByteArrayElements(dataArray, data, 0);
}