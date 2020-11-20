//
// Created by jiaqu on 2020/11/15.
//

#ifndef NATIVEOPENGLDEMO_BASEOPENGL_H
#define NATIVEOPENGLDEMO_BASEOPENGL_H

#include <stdio.h>
#include <string.h>
#include "GLES2/gl2.h"
#include "../log/androidLog.h"
#include "../matrix/MatrixUtil.h"
#include "../shaderutil/shaderUtil.h"

#define GET_STR(x) #x

class BaseOpengl {
public:
    BaseOpengl();

    ~BaseOpengl();

    virtual void onCreate();

    virtual void onChange(int w, int h);

    virtual void onDraw();

    virtual void destroy();

    virtual void setPixel(void *data, int width, int height, int length);

    virtual void setYuvData(void* y, void* u, void *v, int width, int height);

    virtual void destroySource();

public:
    int surface_width = 0;//屏幕宽
    int surface_height = 0;//屏幕高

    char *vertexStr = NULL;//顶点着色器代码
    char *fragmentStr = NULL;//片元着色器代码

    float *vertexs = NULL;//顶点坐标
    float *fragments = NULL;//纹理坐标

    GLuint program = 0;//opengl程序id
    GLuint vShader = 0;
    GLuint fShader = 0;
};

#endif //NATIVEOPENGLDEMO_BASEOPENGL_H
