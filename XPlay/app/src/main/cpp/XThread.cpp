//
// Created by jiaqu on 2020/4/6.
//
#include "XThread.h"
#include "XLog.h"
#include <thread>

//启动线程
void XThread::Start(){
    std::thread th(&XThread::ThreadMain, this);
    th.detach();
}

//通过控制isExit变量安全停止线程(不一定)
void XThread::Stop(){

}

void XThread::ThreadMain(){
    XLOGI("线程函数进入");
    Main();
    XLOGI("线程函数退出");
}