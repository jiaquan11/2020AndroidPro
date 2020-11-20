package com.jiaquan.jnistring;

public class JNIString {
    static {
        System.loadLibrary("jstring-lib");
    }

    public native String callNativeString(String str);

    public native void stringMethod(String str);
}
