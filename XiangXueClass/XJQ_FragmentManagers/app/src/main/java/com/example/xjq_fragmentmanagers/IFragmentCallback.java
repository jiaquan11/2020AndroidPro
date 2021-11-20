package com.example.xjq_fragmentmanagers;

public interface IFragmentCallback {
    void sendMsgToActivity(String msg);
    String getMsgFromActivity(String msg);
}
