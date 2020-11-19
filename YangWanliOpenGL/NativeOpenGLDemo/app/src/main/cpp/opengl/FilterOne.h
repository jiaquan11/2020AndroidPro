//
// Created by jiaqu on 2020/11/15.
//
#ifndef NATIVEOPENGLDEMO_FILTERONE_H
#define NATIVEOPENGLDEMO_FILTERONE_H

#include "BaseOpengl.h"

class FilterOne : public BaseOpengl {
public:
    FilterOne();

    ~FilterOne();

    void onCreate();

    void onChange(int w, int h);

    void onDraw();

    void destroy();

    void setPixel(void *data, int width, int height, int length);

    void destroySource();

private:
    void setMatrix(int width, int height);

public:
    GLint vPosition = 0;
    GLint fPosition = 0;
    GLint sampler = 0;
    GLuint textureID = 0;
    GLint u_matrix = 0;

    int w = 0;//图片宽
    int h = 0;//图片高
    void *pixels = NULL;//图片像素数据

    float matrix[16] = {0};//用于存放单位矩阵
};

#endif //NATIVEOPENGLDEMO_FILTERONE_H
