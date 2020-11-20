//
// Created by jiaqu on 2020/4/6.
//
#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    while (!isExit){
        if (IsPause()){
            XSleep(2);
            continue;
        }
        XData d = Read();
        if (d.size > 0){
            Notify(d);
        }else{
            XSleep(2);
        }
    }
}