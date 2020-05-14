//
// Created by jiaqu on 2020/4/6.
//

#ifndef XPLAY_IOBSERVER_H
#define XPLAY_IOBSERVER_H

#include "XData.h"
#include "XThread.h"
#include <vector>
#include <mutex>

//观察者和主体
class IObserver : public XThread {
public:
    //观察者接收数据函数
    virtual void Update(XData data) {}

    //主体函数 添加观察者（线程安全）
    void AddObs(IObserver *obs);

    //通知所有观察者 （线程安全）
    void Notify(XData data);

protected:
    std::vector<IObserver *> obss;
    std::mutex mux;
};

#endif //XPLAY_IOBSERVER_H
