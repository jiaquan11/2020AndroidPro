//
// Created by jiaqu on 2020/11/8.
//

#include "EglHelper.h"

EglHelper::EglHelper() {
    mEglDisplay = EGL_NO_DISPLAY;
    mEglSurface = EGL_NO_SURFACE;
    mEglContext = EGL_NO_CONTEXT;
    mEglConfig = NULL;
}

EglHelper::~EglHelper() {

}

int EglHelper::initEgl(EGLNativeWindowType window) {
    //1.得到默认的显示设备
    mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (mEglDisplay == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay error");
        return -1;
    }

    //2.初始化默认的显示设备，可以获取得到设备的版本号
    EGLint *version = new EGLint[2];
    if (!eglInitialize(mEglDisplay, &version[0], &version[1])) {
        LOGE("eglInitialize error");
        return -1;
    }

    //3.查询并设置显示设备的属性，调用两次
    const EGLint attribs[] = {//配置属性
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_STENCIL_SIZE, 8,//模板
            EGL_RENDERABLE_TYPE,
            EGL_OPENGL_ES2_BIT,//使用opengles2版本
            EGL_NONE
    };
    EGLint num_config;
    //查询最优config
    if (!eglChooseConfig(mEglDisplay, attribs, NULL, 1, &num_config)) {
        LOGE("eglChooseConfig error");
        return -1;
    }

    //4 以上已查询到config,现直接保存到mEglConfig中
    if (!eglChooseConfig(mEglDisplay, attribs, &mEglConfig, num_config, &num_config)) {
        LOGE("eglChooseConfig error22");
        return -1;
    }

    //5.创建EGLContext
    int attrib_list[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    mEglContext = eglCreateContext(mEglDisplay, mEglConfig, EGL_NO_CONTEXT, attrib_list);
    if (mEglContext == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext error");
        return -1;
    }

    //6.创建渲染的Surface   window:Android传递下来的SurfaceView的窗口，用于显示
    //而mEglSurface是后台缓存
    mEglSurface = eglCreateWindowSurface(mEglDisplay, mEglConfig, window, NULL);
    if (mEglSurface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface error");
        return -1;
    }

    //7.绑定EglContext和Surface到显示设备中   显示上下文和显示界面绑定到显示设备中
    if (!eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
        LOGE("eglMakeCurrent error");
        return -1;
    }
    LOGI("egl init success!");
    return 0;
}

int EglHelper::swapBuffers() {
    if ((mEglDisplay != EGL_NO_DISPLAY) && (mEglSurface != EGL_NO_SURFACE)) {
        //刷新数据，显示渲染场景
        //从后台缓冲mEglSurface将数据渲染到前台显示设备中
        if (!eglSwapBuffers(mEglDisplay, mEglSurface)) {
            LOGE("eglSwapBuffers error");
            return -1;
        }
    }
    return 0;
}

void EglHelper::destroyEgl() {
    if (mEglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if ((mEglDisplay != EGL_NO_DISPLAY) && (mEglSurface != EGL_NO_SURFACE)) {
        eglDestroySurface(mEglDisplay, mEglSurface);
        mEglSurface = EGL_NO_SURFACE;
    }
    if ((mEglDisplay != EGL_NO_DISPLAY) && (mEglContext != EGL_NO_CONTEXT)) {
        eglDestroyContext(mEglDisplay, mEglContext);
        mEglContext = EGL_NO_CONTEXT;
    }
    if (mEglDisplay != EGL_NO_DISPLAY) {
        eglTerminate(mEglDisplay);
        mEglDisplay = EGL_NO_DISPLAY;
    }
}
