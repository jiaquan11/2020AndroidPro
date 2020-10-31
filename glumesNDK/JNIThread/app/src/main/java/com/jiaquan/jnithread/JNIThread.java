package com.jiaquan.jnithread;

public class JNIThread {
    static {
        System.loadLibrary("native-lib");
    }

    public native void createNativeThread();

    public native void createNativeThreadWithArgs();

    public native void joinNativeThread();
}
