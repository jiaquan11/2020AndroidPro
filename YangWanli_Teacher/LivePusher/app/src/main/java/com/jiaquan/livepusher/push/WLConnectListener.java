package com.jiaquan.livepusher.push;

public interface WLConnectListener {
    void onConnecting();

    void onConnectSuccess();

    void onConnectFail(String msg);
}
