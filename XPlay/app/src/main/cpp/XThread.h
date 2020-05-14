//
// Created by jiaqu on 2020/4/6.
//
#ifndef XPLAY_XTHREAD_H
#define XPLAY_XTHREAD_H

//sleep毫秒
void XSleep(int mis);

//c++ 11的线程库
class XThread {
public:
    //启动线程
    virtual bool Start();

    //通过控制isExit变量安全停止线程(不一定)
    virtual void Stop();

    virtual void SetPause(bool isP);

    virtual bool IsPause();

protected:
    //入口主函数
    virtual void Main(){}

    bool isExit = false;

    bool isRunning = false;

    bool isPause = false;
    bool isPausing = false;

private:
    void ThreadMain();
};
#endif //XPLAY_XTHREAD_H
