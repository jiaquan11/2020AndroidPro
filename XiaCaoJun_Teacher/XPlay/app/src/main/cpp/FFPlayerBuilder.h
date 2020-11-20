//
// Created by jiaqu on 2020/4/19.
//

#ifndef XPLAY_FFPLAYERBUILDER_H
#define XPLAY_FFPLAYERBUILDER_H

#include "IPlayerBuilder.h"

class FFPlayerBuilder : public IPlayerBuilder {
public:
    static void InitHard(void *vm);

    static FFPlayerBuilder *Get() {//单例模式
        static FFPlayerBuilder ff;
        return &ff;
    }

protected:
    FFPlayerBuilder() {}

protected:
    virtual IPlayer *CreatePlayer(unsigned char index = 0);

    virtual IDemux *CreateDemux();

    virtual IDecode *CreateDecode();

    virtual IResample *CreateResample();

    virtual IVideoView *CreateVideoView();

    virtual IAudioPlay *CreateAudioPlay();
};

#endif //XPLAY_FFPLAYERBUILDER_H
