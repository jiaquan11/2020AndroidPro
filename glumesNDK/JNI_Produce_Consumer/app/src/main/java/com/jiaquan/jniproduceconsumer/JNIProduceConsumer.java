package com.jiaquan.jniproduceconsumer;

public class JNIProduceConsumer {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native void startProductAndConsumer();
}
