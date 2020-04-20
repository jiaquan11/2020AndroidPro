//
// Created by jiaqu on 2020/4/19.
//

#ifndef XPLAY_FFPLAYERBUILDER_H
#define XPLAY_FFPLAYERBUILDER_H

#include "IPlayerBuilder.h"

class FFPlayerBuilder : public IPlayerBuilder{
public:
    static void InitHard(void* vm);

    static FFPlayerBuilder *Get(){
        static FFPlayerBuilder ff;
        return &ff;
    }

protected:
    FFPlayerBuilder(){}

protected:
    virtual IDemux* CreateDemux();
    virtual IDecode* CreateDecode();
    virtual IResample* CreateResample();
    virtual IVideoView* CreateVideoView();
    virtual IAudioPlay* CreateAudioPlay();
    virtual IPlayer* CreatePlayer(unsigned char index=0);

};
#endif //XPLAY_FFPLAYERBUILDER_H
