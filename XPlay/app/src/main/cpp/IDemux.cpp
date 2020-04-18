//
// Created by jiaqu on 2020/4/6.
//
#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    while (!isExit){
        XData d = Read();
        if (d.size > 0){
            Notify(d);
        }
    }
}