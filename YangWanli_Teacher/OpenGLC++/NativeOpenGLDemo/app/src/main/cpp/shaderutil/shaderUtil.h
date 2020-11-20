//
// Created by jiaqu on 2020/11/10.
//

#ifndef NATIVEOPENGLDEMO_SHADERUTIL_H
#define NATIVEOPENGLDEMO_SHADERUTIL_H

#include <GLES2/gl2.h>
#include "../log/androidLog.h"

static void CheckGLError() {
    GLenum error = glGetError();
    LOGE("CheckGLError error:%d", error);
    if (error != GL_NO_ERROR) {
        switch (error) {
            case GL_INVALID_ENUM:
//                printf("GL Error: GL_INVALID_ENUM %s : %d \n",file,line);
                LOGE("CheckGLError GL_INVALID_ENUM");
                break;
            case GL_INVALID_VALUE:
//                printf("GL Error: GL_INVALID_VALUE %s : %d \n",file,line);
                LOGE("CheckGLError GL_INVALID_VALUE");
                break;
            case GL_INVALID_OPERATION:
//                printf("GL Error: GL_INVALID_OPERATION %s : %d \n",file,line);
                LOGE("CheckGLError GL_INVALID_OPERATION");
                break;
//            case GL_STACK_OVERFLOW:
//                printf("GL Error: GL_STACK_OVERFLOW %s : %d \n",file,line);
//                break;
//            case GL_STACK_UNDERFLOW:
//                printf("GL Error: GL_STACK_UNDERFLOW %s : %d \n",file,line);
//                break;
            case GL_OUT_OF_MEMORY:
//                printf("GL Error: GL_OUT_OF_MEMORY %s : %d \n",file,line);
                LOGE("CheckGLError GL_OUT_OF_MEMORY");
                break;
            default:
//                printf("GL Error: 0x%x %s : %d \n",error,file,line);
                LOGE("CheckGLError default");
                break;
        }
    }
}

static int loadShaders(int shaderType, const char *code) {
    GLuint shader = glCreateShader(shaderType);//创建一个指定类型的shader,并返回一个对应的索引
    glShaderSource(shader, 1, &code, 0);//加载shader源码
    glCompileShader(shader);//编译shader源码

    //获取编译情况
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        LOGE("glCompileShader failed!");
        return 0;
    }
    LOGI("glCompileShader success!");
    return shader;
}

static int createProgram(const char *vertex, const char *fragment, GLuint *v_shader, GLuint *f_shader) {
    GLuint vertexShader = loadShaders(GL_VERTEX_SHADER, vertex);
    LOGI("vertexShader is %d", vertexShader);
    GLuint fragmentShader = loadShaders(GL_FRAGMENT_SHADER, fragment);
    LOGI("fragmentShader is %d", fragmentShader);

    int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    *v_shader = vertexShader;
    *f_shader = fragmentShader;

//    CheckGLError();

//     char buf[256] = {0};
//     int len=0;
//     glGetProgramInfoLog(program, 256, &len, buf);
//     LOGE("buf is %s", buf);

    //获取链接情况
    GLint status;
    glGetShaderiv(program, GL_LINK_STATUS, &status);
    if (status == 0) {
        LOGE("glLinkProgram failed!");
        return 0;
    }
    LOGI("glLinkProgram success!");
    return program;
}

#endif //NATIVEOPENGLDEMO_SHADERUTIL_H
