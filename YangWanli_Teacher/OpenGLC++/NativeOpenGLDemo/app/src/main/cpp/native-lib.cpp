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
Java_com_jiaquan_opengl_NativeOpengl_surfaceChangeFilter(JNIEnv *env, jobject thiz) {
    // TODO: implement surfaceChangeFilter()
    if (opengl != NULL) {
        opengl->onChangeSurfaceFilter();
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

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_setYuvData(JNIEnv *env, jobject thiz, jbyteArray y_array,
                                                jbyteArray u_array, jbyteArray v_array, jint w,
                                                jint h) {
    // TODO: implement setYuvData()
    jbyte *ydata = env->GetByteArrayElements(y_array, NULL);
    jbyte *udata = env->GetByteArrayElements(u_array, NULL);
    jbyte *vdata = env->GetByteArrayElements(v_array, NULL);

    if (opengl != NULL) {
        opengl->setYuvData(ydata, udata, vdata, w, h);
    }

    env->ReleaseByteArrayElements(y_array, ydata, 0);
    env->ReleaseByteArrayElements(u_array, udata, 0);
    env->ReleaseByteArrayElements(v_array, vdata, 0);
}