//
// Created by jiaqu on 2020/4/6.
//
#include "XThread.h"
#include "XLog.h"
#include <thread>
using namespace std;

//sleep毫秒
void XSleep(int mis){
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}

//启动线程
void XThread::Start(){
    isExit = false;
    thread th(&XThread::ThreadMain, this);
    th.detach();
}

//通过控制isExit变量安全停止线程(不一定)
void XThread::Stop(){
    isExit = true;
    for (int i = 0; i < 200; ++i) {
        if (!isRunning){
            XLOGI("Stop 停止线程成功!");
            return;
        }
        XSleep(1);
    }
    XLOGI("Stop 停止线程超时!");
}

void XThread::ThreadMain(){
    XLOGI("线程函数进入");
    isRunning = true;
    Main();
    isRunning = false;
    XLOGI("线程函数退出");
}