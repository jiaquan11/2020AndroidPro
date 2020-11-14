#include "log/androidLog.h"
#include <string>
#include "egl/EglThread.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "GLES2/gl2.h"
#include "shaderutil/shaderUtil.h"
#include "matrix/MatrixUtil.h"

ANativeWindow *nativeWindow = NULL;
EglThread *eglThread = NULL;

#define GET_STR(x) #x
const char* vertexStr = GET_STR(
        attribute vec4 v_Position;
           attribute vec2 f_Position;
           varying vec2 ft_Position;
           uniform mat4 u_Matrix;
           void main(){
               ft_Position = f_Position;
               gl_Position = v_Position * u_Matrix;//将矩阵与顶点坐标进行相乘，用于图像旋转
        });

const char* fragmentStr = GET_STR(
        precision mediump float;
           varying vec2 ft_Position;
           uniform sampler2D sTexture;
           void main(){//texture2D表示GPU将输入得图像纹理像素进行读取，读取到GPU的管线中,最后渲染出来
            gl_FragColor = texture2D(sTexture,ft_Position);
           });

int program = 0;
GLint vPosition = 0;
GLint fPosition = 0;
GLint sampler = 0;
GLuint textureID = 0;
GLint u_matrix = 0;

int w;
int h;
void *pixels = NULL;

float vertexs[] = {
        1, -1,
        1, 1,
        -1, -1,
        -1, 1,
};

float fragments[] = {
        1, 1,
        1, 0,
        0, 1,
        0, 0
};

float matrix[16];

void callback_SurfaceCreate(void *ctx) {
    LOGI("callback_SurfaceCreate");
    EglThread *eglThread = static_cast<EglThread *>(ctx);

    program = createProgram(vertexStr, fragmentStr);
    LOGI("callback_SurfaceCreate GET_STR opengl program: %d", program);

    //获取着色器程序中的这个变量a_position，返回一个变量id，用于给这个变量赋值
    vPosition = glGetAttribLocation(program, "v_Position");//顶点坐标变量
    fPosition = glGetAttribLocation(program, "f_Position");//纹理坐标变量
    sampler = glGetUniformLocation(program, "sTexture");//2D纹理变量
    u_matrix = glGetUniformLocation(program, "u_Matrix");//着色器矩阵变量

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);//绑定纹理

    LOGI("textureID is %d", textureID);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    if (pixels != NULL){//为后台缓存显存中设置图片数据
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//    }
    glBindTexture(GL_TEXTURE_2D, 0);//解绑纹理

    initMatrix(matrix);//初始化矩阵
    //rotateMatrix(90, matrix);//旋转矩阵，这里是逆时针90度旋转，-90是顺时针旋转90度
    scaleMatrix(0.5, matrix);//图像缩放，一般是对X轴和Y轴等值缩放，0.5缩小一倍
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

    //渲染时纹理赋值操作
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(sampler, 0);//GL_TEXTURE0表示就是第一层纹理

    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, matrix);//给矩阵变量赋值

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (pixels != NULL){//为后台缓存显存中设置图片数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }

    //渲染时顶点赋值操作
    glEnableVertexAttribArray(vPosition);//使能这个着色器变量

    /*给着色器的顶点顶点变量赋值
     * 第一个参数是着色器的变量id,第二个参数是每个顶点两个值，第三个参数是值的类型，第四个参数表示是否
    归一化，已经有顶点参数，无需自动归一化。第五个参数表示每个顶点的跨度(这里每个顶点跨8个字节)，
     第六个参数表示顶点数组
     */
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, false, 8, vertexs);

    glEnableVertexAttribArray(fPosition);
    glVertexAttribPointer(fPosition, 2, GL_FLOAT, false, 8, fragments);
    /*opengl绘制
     * 绘制三角形，第二个参数表示从索引0开始，绘制三个顶点
     */
//    glDrawArrays(GL_TRIANGLES, 0, 3);//绘制三角形
//    glDrawArrays(GL_TRIANGLES, 0, 6);//绘制四边形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//绘制四边形

    glBindTexture(GL_TEXTURE_2D, 0);
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

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_opengl_NativeOpengl_imgData(JNIEnv *env, jobject thiz, jint width, jint height, jint length,
                                             jbyteArray dataArray) {
    // TODO: implement imgData()
    jbyte* data = env->GetByteArrayElements(dataArray, NULL);

    w = width;
    h = height;
    pixels = malloc(length);
    memcpy(pixels, data, length);

    env->ReleaseByteArrayElements(dataArray, data, 0);
}