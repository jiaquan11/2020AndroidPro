//
// Created by jiaqu on 2020/11/25.
//

#include "WLAudio.h"

WLAudio::WLAudio(WLPlayStatus* playStatus) {
    this->playStatus = playStatus;
    queue = new WLQueue(playStatus);
}

WLAudio::~WLAudio() {

}
