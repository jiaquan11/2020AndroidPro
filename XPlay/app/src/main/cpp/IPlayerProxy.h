//
// Created by jiaqu on 2020/4/19.
//

#ifndef XPLAY_IPLAYERPROXY_H
#define XPLAY_IPLAYERPROXY_H

#include <mutex>
#include "IPlayer.h"

class IPlayerProxy : public IPlayer{
public:
    static IPlayerProxy* Get(){
        static IPlayerProxy px;
        return &px;
    }

    void Init(void *vm = 0);

    virtual bool Open(const char* path);
    virtual bool Start();
    virtual void InitView(void* win);

protected:
    IPlayerProxy(){}

protected:
    IPlayer* player = 0;
    std::mutex mux;
};
#endif //XPLAY_IPLAYERPROXY_H
