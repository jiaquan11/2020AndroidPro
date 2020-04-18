//
// Created by jiaqu on 2020/4/11.
//

#ifndef XPLAY_XTEXTURE_H
#define XPLAY_XTEXTURE_H

class XTexture {
public:
    static XTexture* Create();

    virtual bool Init(void *win) = 0;

    virtual void Draw(unsigned char* data[], int width, int height) = 0;
};
#endif //XPLAY_XTEXTURE_H
