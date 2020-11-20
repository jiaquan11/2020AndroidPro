package com.jiaquan.jniinvokemethod;

public class JNIInvokeMethod {
    static {
        System.loadLibrary("native-lib");
    }

    public native void nativeCallback(ICallbackMethod callbackMethod);

    public native void nativeThreadCallback(IThreadCallback callback);
}
