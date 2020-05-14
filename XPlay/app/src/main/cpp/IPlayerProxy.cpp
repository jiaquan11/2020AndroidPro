//
// Created by jiaqu on 2020/4/19.
//
#include "IPlayerProxy.h"
#include "FFPlayerBuilder.h"

void IPlayerProxy::Init(void *vm) {
    mux.lock();
    if (vm) {
        FFPlayerBuilder::InitHard(vm);
    }

    if (!player)
        player = FFPlayerBuilder::Get()->BuilderPlayer();
    mux.unlock();
}

void IPlayerProxy::Close() {
    mux.lock();
    if (player) {
        player->Close();
    }
    mux.unlock();
}

bool IPlayerProxy::IsPause() {
    bool ret = false;
    mux.lock();
    if (player) {
        ret = player->IsPause();
    }
    mux.unlock();
    return ret;
}

void IPlayerProxy::SetPause(bool isP) {
    mux.lock();
    if (player) {
        player->SetPause(isP);
    }
    mux.unlock();
}

//获取当前的播放进度 0.0-1.0
double IPlayerProxy::PlayPos() {
    double pos = 0.0;
    mux.lock();
    if (player) {
        pos = player->PlayPos();
    }
    mux.unlock();
    return pos;
}

bool IPlayerProxy::Seek(double pos) {
    bool ret = false;
    mux.lock();
    if (player) {
        ret = player->Seek(pos);
    }
    mux.unlock();
    return ret;
}

bool IPlayerProxy::Open(const char *path) {
    bool ret = false;
    mux.lock();
    if (player) {
        player->isHardDecode = isHardDecode;
        ret = player->Open(path);
    }
    mux.unlock();
    return ret;
}

bool IPlayerProxy::Start() {
    bool ret = false;
    mux.lock();
    if (player)
        ret = player->Start();
    mux.unlock();
    return ret;
}

void IPlayerProxy::InitView(void *win) {
    mux.lock();
    if (player)
        player->InitView(win);
    mux.unlock();
}