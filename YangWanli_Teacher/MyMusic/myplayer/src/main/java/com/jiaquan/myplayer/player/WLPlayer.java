package com.jiaquan.myplayer.player;

import android.text.TextUtils;

import com.jiaquan.myplayer.TimeInfoBean;
import com.jiaquan.myplayer.listener.OnLoadListener;
import com.jiaquan.myplayer.listener.OnPauseResumeListener;
import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.listener.OnTimeInfoListener;
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

    private OnPreparedListener onPreparedListener = null;
    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    private OnLoadListener onLoadListener = null;
    public void setOnLoadListener(OnLoadListener onLoadListener) {
        this.onLoadListener = onLoadListener;
    }

    private OnPauseResumeListener onPauseResumeListener = null;
    public void setOnPauseResumeListener(OnPauseResumeListener onPauseResumeListener) {
        this.onPauseResumeListener = onPauseResumeListener;
    }

    private OnTimeInfoListener onTimeInfoListener = null;
    public void setOnTimeInfoListener(OnTimeInfoListener onTimeInfoListener) {
        this.onTimeInfoListener = onTimeInfoListener;
    }

    private static TimeInfoBean timeInfoBean = null;

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

        onCallLoad(true);

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

    public void onCallLoad(boolean load) {
        if (onLoadListener != null) {
            onLoadListener.onLoad(load);
        }
    }

    public void onCallTimeInfo(int currentTime, int totalTime){
        if (onTimeInfoListener != null){
            if (timeInfoBean == null){
                timeInfoBean = new TimeInfoBean();
            }
            timeInfoBean.setCurrentTime(currentTime);
            timeInfoBean.setTotalTime(totalTime);
            onTimeInfoListener.onTimeInfo(timeInfoBean);
        }
    }

    public void pause() {
        _pause();
        if (onPauseResumeListener != null) {
            onPauseResumeListener.onPause(true);
        }
    }

    public void resume() {
        _resume();
        if (onPauseResumeListener != null) {
            onPauseResumeListener.onPause(false);
        }
    }

    private native void _prepared(String source);

    private native void _start();

    private native void _pause();

    private native void _resume();
}
