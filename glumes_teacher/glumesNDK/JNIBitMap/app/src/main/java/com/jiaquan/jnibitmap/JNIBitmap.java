package com.jiaquan.jnibitmap;

import android.graphics.Bitmap;

public class JNIBitmap {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native Bitmap callNativeMirrorBitmap(Bitmap bitmap);
}
