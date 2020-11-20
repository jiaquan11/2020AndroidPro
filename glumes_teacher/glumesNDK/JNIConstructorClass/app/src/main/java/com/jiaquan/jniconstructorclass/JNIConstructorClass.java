package com.jiaquan.jniconstructorclass;

public class JNIConstructorClass {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native Animal allocObjectConstructor();

    public native Animal invokeAnimalConstructors();
}
