//
// Created by jiaqu on 2020/4/11.
//

#ifndef XPLAY_GLVIDEOVIEW_H
#define XPLAY_GLVIDEOVIEW_H

#include "XData.h"
#include "IVideoView.h"

class XTexture;

class GLVideoView : public IVideoView{
public:
    virtual void SetRender(void* win);

    virtual void Render(XData data);

protected:
    void* view = 0;
    XTexture* txt = 0;
};


#endif //XPLAY_GLVIDEOVIEW_H
