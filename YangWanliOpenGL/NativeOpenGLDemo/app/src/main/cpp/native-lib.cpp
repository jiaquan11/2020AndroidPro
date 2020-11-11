#include "log/androidLog.h"
#include <string>
#include "egl/EglThread.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "GLES2/gl2.h"
#include "shaderutil/shaderUtil.h"

ANativeWindow *nativeWindow = NULL;
EglThread *eglThread = NULL;

#define GET_STR(x) #x
const char* vertexStr = GET_STR(
        attribute vec4 a_position;
        void main(){
            gl_Position = a_position;
        });

const char* fragmentStr = GET_STR(
        precision mediump float;
        void main() {
            gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        });

//const char* vertex = "attribute vec4 a_position;\n"
//                     "\n"
//                     "void main(){\n"
//                     "gl_Position = a_position;\n"
//                     "}";
//
//const char* fragment = "precision mediump float;\n"
//                       "\n"
//                       "void main() {\n"
//                       "gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
//                       "}";

int program = 0;
GLint vPosition = 0;

float vertexs[] = {
        -1, -1,
        1, -1,
        0, 1
};
void callback_SurfaceCreate(void *ctx) {
    LOGI("callback_SurfaceCreate");
    EglThread *eglThread = static_cast<EglThread *>(ctx);

    program = createProgram(vertexStr, fragmentStr);
    LOGI("callback_SurfaceCreate GET_STR opengl program: %d", program);

    //获取着色器程序中的这个变量a_position，返回一个变量id，用于给这个变量赋值
    vPosition = glGetAttribLocation(program, "a_position");
}

void callback_SurfaceChange(int w, int h, void *ctx) {
    LOGI("callback_SurfaceChange");
    EglThread *eglThread = static_cast<EglThread *>(ctx);
    glViewport(0, 0, w, h);
}

void callback_SurfaceDraw(void *ctx) {
    LOGI("callback_SurfaceDraw");
    EglThread *eglThread = static_cast<EglThread *>(ctx);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//指定刷屏颜色  1:不透明  0：透明
    glClear(GL_COLOR_BUFFER_BIT);//将刷屏颜色进行刷屏，但此时仍然处于后台缓冲中，需要swapBuffers交换到前台界面显示

    glUseProgram(program);//使用着色器程序
    glEnableVertexAttribArray(vPosition);//使能这个着色器变量

    /*给着色器的顶点顶点变量赋值
     * 第一个参数是着色器的变量id,第二个参数是每个顶点两个值，第三个参数是值的类型，第四个参数表示是否
    归一化，已经有顶点参数，无需自动归一化。第五个参数表示每个顶点的跨度(这里每个顶点跨8个字节)，
     第六个参数表示顶点数组
     */
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, false, 8, vertexs);

    /*opengl绘制
     * 绘制三角形，第二个参数表示从索引0开始，绘制三个顶点
     */
    glDrawArrays(GL_TRIANGLES, 0, 3);
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