package com.jiaquan.livepusher.push;

import android.content.Context;

import com.jiaquan.livepusher.encodec.WLBaseMediaEncoder;

public class WLPushEncoder extends WLBasePushEncoder {
    private WLEncoderPushRender wlEncoderPushRender;

    public WLPushEncoder(Context context, int textureId) {
        super(context);
        wlEncoderPushRender = new WLEncoderPushRender(context, textureId);

        setRender(wlEncoderPushRender);
        setmRenderMode(WLBaseMediaEncoder.RENDERMODE_CONTINUOUSLY);
    }
}
