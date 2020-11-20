//
// Created by jiaqu on 2020/4/19.
//
#include "IPlayerBuilder.h"
#include "IVideoView.h"
#include "IResample.h"
#include "IDecode.h"
#include "IAudioPlay.h"
#include "IDemux.h"

IPlayer *IPlayerBuilder::BuilderPlayer(unsigned char index) {
    IPlayer *player = CreatePlayer(index);

    //解封装
    IDemux *demux = CreateDemux();

    //视频解码
    IDecode *vdecode = CreateDecode();
    //音频解码
    IDecode *adecode = CreateDecode();
    //解码器观察解封装
    demux->AddObs(vdecode);
    demux->AddObs(adecode);

    //显示观察视频解码器
    IVideoView *view = CreateVideoView();
    vdecode->AddObs(view);

    //重采样观察音频解码器
    IResample *resample = CreateResample();
    adecode->AddObs(resample);

    //音频播放观察重采样
    IAudioPlay *audioPlay = CreateAudioPlay();
    resample->AddObs(audioPlay);

    player->demux = demux;
    player->vdecode = vdecode;
    player->adecode = adecode;
    player->videoView = view;
    player->resample = resample;
    player->audioPlay = audioPlay;

    return player;
}