//
// Created by jiaqu on 2020/4/19.
//
#include "IPlayerProxy.h"
#include "FFPlayerBuilder.h"

void IPlayerProxy::Init(void* vm) {
    mux.lock();
    if (vm){
        FFPlayerBuilder::InitHard(vm);
    }

    if (!player)
        player = FFPlayerBuilder::Get()->BuilderPlayer();
    mux.unlock();
}

bool IPlayerProxy::Open(const char* path){
    bool ret = false;
    mux.lock();
    if (player)
        ret = player->Open(path);
    mux.unlock();
    return ret;
}

bool IPlayerProxy::Start(){
    bool ret = false;
    mux.lock();
    if (player)
        ret = player->Start();
    mux.unlock();
    return ret;
}

void IPlayerProxy::InitView(void* win){
    mux.lock();
    if (player)
        player->InitView(win);
    mux.unlock();
}