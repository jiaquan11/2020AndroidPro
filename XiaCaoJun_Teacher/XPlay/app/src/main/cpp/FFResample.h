//
// Created by jiaqu on 2020/4/18.
//

#ifndef XPLAY_FFRESAMPLE_H
#define XPLAY_FFRESAMPLE_H

#include "IResample.h"
struct SwrContext;

class FFResample : public IResample{
public:
    virtual bool Open(XParameter in, XParameter out = XParameter());
    virtual void Close();
    virtual XData Resample(XData indata);

protected:
    SwrContext *actx = 0;
    std::mutex mux;
};
#endif //XPLAY_FFRESAMPLE_H
