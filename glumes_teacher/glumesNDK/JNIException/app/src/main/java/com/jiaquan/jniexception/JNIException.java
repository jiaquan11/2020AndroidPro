package com.jiaquan.jniexception;

public class JNIException {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native void nativeInvokeJavaException();//JNI捕获Java异常

    public native void nativeThrowException() throws IllegalArgumentException;//JNI层抛出异常

    public int operation (){
        return 2/0;
    }
}
