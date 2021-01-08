//
// Created by jiaqu on 2020/11/26.
//

#ifndef MYMUSIC_WLPLAYSTATUS_H
#define MYMUSIC_WLPLAYSTATUS_H

class WLPlayStatus {
public:
    WLPlayStatus();

    ~WLPlayStatus();

public:
    bool isExit = false;
    bool load = true;
    bool seek = false;
    bool pause = false;
};


#endif //MYMUSIC_WLPLAYSTATUS_H
