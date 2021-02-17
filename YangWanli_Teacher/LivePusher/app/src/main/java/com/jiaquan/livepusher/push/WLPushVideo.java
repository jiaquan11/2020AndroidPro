package com.jiaquan.livepusher.push;

import android.text.TextUtils;

public class WLPushVideo {
    static {
        System.loadLibrary("wlpush");
    }

    public void initLivePush(String url){
        if (!TextUtils.isEmpty(url)){
            initPush(url);
        }
    }

    private native void initPush(String pushUrl);
}
