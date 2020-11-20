//
// Created by jiaqu on 2020/11/19.
//

#ifndef NATIVEOPENGLDEMO_FILTERYUV_H
#define NATIVEOPENGLDEMO_FILTERYUV_H

#include "BaseOpengl.h"

class FilterYUV : public BaseOpengl{
public:
    FilterYUV();
    ~FilterYUV();

    void onCreate();

    void onChange(int w, int h);

    void onDraw();

    void destroy();

    void destroySource();

    void setYuvData(void *Y, void *U, void *V, int width, int height);

private:
    void setMatrix(int width, int height);

public:
    GLint vPosition = 0;
    GLint fPosition = 0;
    GLint u_matrix = 0;

    GLint sampler_y = 0;
    GLint sampler_u = 0;
    GLint sampler_v = 0;

    GLuint samplers[3] = {0};
    float matrix[16] = {0};

    void* y = NULL;
    void* u = NULL;
    void* v = NULL;

    int yuv_width = 0;
    int yuv_height = 0;
};
#endif //NATIVEOPENGLDEMO_FILTERYUV_H
