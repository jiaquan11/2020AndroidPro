//
// Created by jiaqu on 2020/4/11.
//
#ifndef XPLAY_XSHADER_H
#define XPLAY_XSHADER_H

class XShader {
public:
    virtual bool Init();

    //获取材质并映射到内存
    virtual void GetTexture(unsigned int index, int widht, int height, unsigned char* buf);

    virtual void Draw();

protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    unsigned int texts[100] = {0};
};
#endif //XPLAY_XSHADER_H
