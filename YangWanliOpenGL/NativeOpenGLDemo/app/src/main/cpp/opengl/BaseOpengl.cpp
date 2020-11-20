//
// Created by jiaqu on 2020/11/15.
//

#include "BaseOpengl.h"

BaseOpengl::BaseOpengl() {//顶点坐标，原点在中间，范围-1-1
    vertexs = new float[8];
    fragments = new float[8];

    float v[] = {
            1, -1,
            1, 1,
            -1, -1,
            -1, 1
    };
    memcpy(vertexs, v, sizeof(v));

    float f[] = {//纹理坐标：原点在左上角：0-1
            1, 1,
            1, 0,
            0, 1,
            0, 0
    };
    memcpy(fragments, f, sizeof(f));
}

BaseOpengl::~BaseOpengl() {
    delete[]vertexs;
    delete[]fragments;
}

void BaseOpengl::onCreate() {

}

void BaseOpengl::onChange(int w, int h) {

}

void BaseOpengl::onDraw() {

}

void BaseOpengl::destroy() {

}

void BaseOpengl::setPixel(void *data, int width, int height, int length) {

}

void BaseOpengl::setYuvData(void *y, void *u, void *v, int width, int height) {

}

void BaseOpengl::destroySource() {

}





