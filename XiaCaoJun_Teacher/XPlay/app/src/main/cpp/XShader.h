//
// Created by jiaqu on 2020/4/11.
//
#ifndef XPLAY_XSHADER_H
#define XPLAY_XSHADER_H

#include <mutex>

enum XShaderType{
    XSHADER_YUV420P = 0,//软解码和虚拟机
    XSHADER_NV12 = 25,//手机
    XSHADER_NV21 = 26,
};

class XShader {
public:
    virtual bool Init(XShaderType type = XSHADER_YUV420P);

    virtual void Close();

    //获取材质并映射到内存
    virtual void GetTexture(unsigned int index, int widht, int height, unsigned char* buf, bool isa= false);

    virtual void Draw();

protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    unsigned int texts[100] = {0};

    std::mutex mux;
};
#endif //XPLAY_XSHADER_H
