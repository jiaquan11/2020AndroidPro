//
// Created by jiaqu on 2020/11/15.
//

#ifndef NATIVEOPENGLDEMO_BASEOPENGL_H
#define NATIVEOPENGLDEMO_BASEOPENGL_H

#include <stdio.h>
#include <string.h>
#include "GLES2/gl2.h"

class BaseOpengl {
public:
    BaseOpengl();
    ~BaseOpengl();

    virtual void onCreate();
    virtual void onChange(int w, int h);
    virtual void onDraw();

    virtual void setPixel(void* data, int width, int height, int length);

public:
    int surface_width = 0;//屏幕宽
    int surface_height = 0;//屏幕高

    char* vertexStr = NULL;//顶点着色器代码
    char* fragmentStr = NULL;//片元着色器代码

    float* vertexs = NULL;//顶点坐标
    float* fragments = NULL;//纹理坐标

    GLuint program = 0;//opengl程序id
};
#endif //NATIVEOPENGLDEMO_BASEOPENGL_H
