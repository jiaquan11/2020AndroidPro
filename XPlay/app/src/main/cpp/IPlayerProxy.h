//
// Created by jiaqu on 2020/4/19.
//

#ifndef XPLAY_IPLAYERPROXY_H
#define XPLAY_IPLAYERPROXY_H

#include <mutex>
#include "IPlayer.h"

class IPlayerProxy : public IPlayer {
public:
    static IPlayerProxy *Get() {//单例模式
        static IPlayerProxy px;
        return &px;
    }

    void Init(void *vm = 0);

    virtual bool Open(const char *path);

    virtual void Close();

    virtual bool Seek(double pos);

    virtual bool Start();

    virtual void InitView(void *win);

    //获取当前的播放进度 0.0-1.0
    virtual double PlayPos();

    virtual void SetPause(bool isP);

    virtual bool IsPause();

protected:
    IPlayerProxy() {}

protected:
    IPlayer *player = 0;
    std::mutex mux;
};
#endif //XPLAY_IPLAYERPROXY_H
