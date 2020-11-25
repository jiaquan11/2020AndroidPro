package com.jiaquan.jnithread;

public class ThreadDemo {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native void normalThread();

    public native void mutexThread();
}
