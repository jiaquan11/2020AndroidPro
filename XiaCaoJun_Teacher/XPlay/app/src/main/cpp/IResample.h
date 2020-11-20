//
// Created by jiaqu on 2020/4/18.
//

#ifndef XPLAY_IRESAMPLE_H
#define XPLAY_IRESAMPLE_H

#include "IObserver.h"
#include "XParameter.h"

class IResample : public IObserver{
public:
    virtual bool Open(XParameter in, XParameter out = XParameter()) = 0;
    virtual XData Resample(XData indata) = 0;
    virtual void Close() = 0;
    virtual void Update(XData data);
    int outChannels = 2;
    int outFormat = 1;
};
#endif //XPLAY_IRESAMPLE_H
