//
// Created by jiaqu on 2020/4/19.
//
#include "IAudioPlay.h"
#include "XLog.h"

XData IAudioPlay::GetData() {
    XData d;
    while (!isExit){
        framesMutex.lock();
        if (!frames.empty()){
            //有数据返回
            d = frames.front();
            frames.pop_front();
            framesMutex.unlock();

            pts = d.pts;
            return d;
        }
        framesMutex.unlock();
        XSleep(1);
    }

    //未获取数据
    return d;
}

void IAudioPlay::Update(XData data){
    //压入缓冲队列
    if (data.size <= 0 || !data.data) return;

    while (!isExit){
        framesMutex.lock();
        if (frames.size() > maxFrames){
            framesMutex.unlock();
            XSleep(1);
            continue;
        }
        frames.push_back(data);
        framesMutex.unlock();
        break;
    }
}