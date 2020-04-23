//
// Created by jiaqu on 2020/4/11.
//
#include <android/native_window.h>
#include <EGL/egl.h>
#include <mutex>
#include "XLog.h"
#include "XEGL.h"

class CXEGL : public XEGL{
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    std::mutex mux;

public:
    virtual void Close(){
        mux.lock();
        if (display == EGL_NO_DISPLAY){
            mux.unlock();
            return;
        }
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (surface != EGL_NO_SURFACE)
            eglDestroySurface(display, surface);
        if (context != EGL_NO_CONTEXT)
            eglDestroyContext(display, context);

        eglTerminate(display);
        display = EGL_NO_DISPLAY;
        surface = EGL_NO_SURFACE;
        context = EGL_NO_CONTEXT;

        mux.unlock();
    }

    virtual bool Init(void* win){
        ANativeWindow* nwin = (ANativeWindow*)win;

        Close();

        mux.lock();
        //初始化EGL
        //1 获取EGLDisplay对象，显示设备
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY){
            mux.unlock();
            XLOGE("eglGetDisplay failed!");
            return false;
        }
        XLOGI("eglGetDisplay success!");

        //2.初始化Display
        if (EGL_TRUE != eglInitialize(display, 0, 0)){
            mux.unlock();
            XLOGE("eglInitialize failed!");
            return false;
        }
        XLOGI("eglInitialize success!");

        //3.获取配置并创建surface
        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_NONE
        };
        EGLConfig config = 0;
        EGLint numConfigs = 0;
        if (EGL_TRUE != eglChooseConfig(display, configSpec, &config, 1, &numConfigs)){
            mux.unlock();
            XLOGE("eglChooseConfig failed!");
            return false;
        }
        XLOGI("eglChooseConfig success!");

        surface = eglCreateWindowSurface(display, config, nwin, NULL);
        if (surface == EGL_NO_SURFACE){
            mux.unlock();
            XLOGE("eglCreateWindowSurface failed!");
            return false;
        }

        //4 创建并打开EGL上下文
        const EGLint ctxAttr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
        if (context == EGL_NO_CONTEXT){
            mux.unlock();
            XLOGE("eglCreateContext failed!");
            return false;
        }
        XLOGI("eglCreateContext success!");

        if (EGL_TRUE != eglMakeCurrent(display, surface, surface, context)){
            mux.unlock();
            XLOGE("eglMakeCurrent failed!");
            return false;
        }
        XLOGI("eglMakeCurrent success!");
        mux.unlock();
        return true;
    }

    virtual void Draw(){
        mux.lock();
        if ((display == EGL_NO_DISPLAY) || (surface == EGL_NO_SURFACE)){
            mux.unlock();
            return;
        }

        eglSwapBuffers(display, surface);
        mux.unlock();
    }
};

XEGL* XEGL::Get(){
    static CXEGL egl;
    return &egl;
}

XEGL::XEGL(){

}
