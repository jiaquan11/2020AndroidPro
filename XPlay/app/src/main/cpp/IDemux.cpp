//
// Created by jiaqu on 2020/4/6.
//
#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    for (;;){
        XData d = Read();
        XLOGI("IDemux Read %d", d.size);
        if (d.size <= 0){
            break;
        }
    }
}