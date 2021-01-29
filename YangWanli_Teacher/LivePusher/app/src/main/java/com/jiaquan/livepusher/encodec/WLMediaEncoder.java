package com.jiaquan.livepusher.encodec;

import android.content.Context;

public class WLMediaEncoder extends WLBaseMediaEncoder {
    private WLEncoderRender wlEncoderRender;

    public WLMediaEncoder(Context context, int textureId) {
        super(context);
        wlEncoderRender = new WLEncoderRender(context, textureId);

        setRender(wlEncoderRender);
        setmRenderMode(WLBaseMediaEncoder.RENDERMODE_CONTINUOUSLY);
    }
}
