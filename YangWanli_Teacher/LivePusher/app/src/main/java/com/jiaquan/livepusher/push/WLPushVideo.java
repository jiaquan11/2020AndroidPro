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

    public void pushSPSPPS(byte[] sps, byte[] pps) {
        if ((sps != null) && (pps != null)) {
            pushSPSPPS(sps, sps.length, pps, pps.length);
        }
    }

    public void pushVideoData(byte[] data, boolean keyframe) {
        if (data != null) {
            pushVideoData(data, data.length, keyframe);
        }
    }

    public void pushAudioData(byte[] data) {
        if (data != null) {
            pushAudioData(data, data.length);
        }
    }

    public void stopPush() {
        pushStop();
    }

    private native void initPush(String pushUrl);

    private native void pushSPSPPS(byte[] sps, int sps_len, byte[] pps, int pps_len);

    private native void pushVideoData(byte[] data, int data_len, boolean keyframe);

    private native void pushAudioData(byte[] data, int data_len);

    private native void pushStop();
}
