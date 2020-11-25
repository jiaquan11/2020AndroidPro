package com.jiaquan.jnithread;

public class ThreadDemo {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native void normalThread();

    public native void mutexThread();

    private OnErrorListener onErrorListener;

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    public void onError(int code, String msg) {
        if (onErrorListener != null) {
            onErrorListener.onError(code, msg);
        }
    }

    public interface OnErrorListener {
        void onError(int code, String msg);
    }

    public native void callBackFromC();
}
