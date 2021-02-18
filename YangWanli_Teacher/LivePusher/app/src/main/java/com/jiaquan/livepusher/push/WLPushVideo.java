package com.jiaquan.livepusher.push;

import android.text.TextUtils;

public class WLPushVideo {
    static {
        System.loadLibrary("wlpush");
    }

    public void setWlConnectListener(WLConnectListener wlConnectListener) {
        this.wlConnectListener = wlConnectListener;
    }

    private WLConnectListener wlConnectListener;

    private void onConnecting() {
        if (wlConnectListener != null) {
            wlConnectListener.onConnecting();
        }
    }

    private void onConnectSuccess() {
        if (wlConnectListener != null) {
            wlConnectListener.onConnectSuccess();
        }
    }

    private void onConnectFail(String msg) {
        if (wlConnectListener != null) {
            wlConnectListener.onConnectFail(msg);
        }
    }

    public void initLivePush(String url) {
        if (!TextUtils.isEmpty(url)) {
            initPush(url);
        }
    }

    private native void initPush(String pushUrl);
}
