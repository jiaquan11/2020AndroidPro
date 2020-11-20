package com.jiaquan.jnithreadsynchronize;

public class JNIThreadSYN {
    static {
        System.loadLibrary("native-lib");
    }

    public native void waitNativeThread();

    public native void notifyNativeThread();
}
