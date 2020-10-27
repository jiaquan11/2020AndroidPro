package com.jiaquan.jniaccessfield;

public class JNIAccessField {
    static {
        System.loadLibrary("native-lib");
    }

    public static int num;
    public native void accessInstanceField(Animal animal);
    public native void accessStaticField(Animal animal);
    public static native void staticAccessInstanceField();

    public native void accessInstanceMethod(Animal animal);
    public native void accessStaticMethod(Animal animal);
}
