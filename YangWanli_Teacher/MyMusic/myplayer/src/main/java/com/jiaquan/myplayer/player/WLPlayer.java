package com.jiaquan.myplayer.player;

import android.text.TextUtils;

import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.log.MyLog;

public class WLPlayer {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }

    private String source = null;

    private OnPreparedListener onPreparedListener;

    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public WLPlayer() {

    }

    public void prepared() {
        if (TextUtils.isEmpty(source)) {
            MyLog.i("source must not be empty");
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                _prepared(source);
            }
        }).start();
    }

    public void start() {
        if (TextUtils.isEmpty(source)) {
            MyLog.i("source must not be empty");
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                _start();
            }
        }).start();
    }

    public void onCallPrepared() {
        if (onPreparedListener != null) {
            onPreparedListener.onPrepared();
        }
    }

    public native void _prepared(String source);

    public native void _start();
}
