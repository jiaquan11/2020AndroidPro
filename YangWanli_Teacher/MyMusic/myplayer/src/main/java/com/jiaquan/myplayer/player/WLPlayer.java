package com.jiaquan.myplayer.player;

import android.text.TextUtils;

import com.jiaquan.myplayer.TimeInfoBean;
import com.jiaquan.myplayer.listener.OnCompleteListener;
import com.jiaquan.myplayer.listener.OnErrorListener;
import com.jiaquan.myplayer.listener.OnLoadListener;
import com.jiaquan.myplayer.listener.OnPauseResumeListener;
import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.listener.OnTimeInfoListener;
import com.jiaquan.myplayer.log.MyLog;
import com.jiaquan.myplayer.muteenum.MuteEnum;

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

    private static String source = null;
    private static boolean playNext = false;
    private static int duration = -1;
    private static int volumePercent = 100;
    private static MuteEnum muteEnum = MuteEnum.MUTE_CENTER;
    private static float speed = 1.0f;
    private static float pitch = 1.0f;

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

    private OnErrorListener onErrorListener = null;

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    private OnCompleteListener onCompleteListener = null;

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
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

//        onCallLoad(true);

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
                setVolume(volumePercent);
                setMute(muteEnum);
                setPitch(pitch);
                setSpeed(speed);
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

    public void onCallTimeInfo(int currentTime, int totalTime) {
        if (onTimeInfoListener != null) {
            if (timeInfoBean == null) {
                timeInfoBean = new TimeInfoBean();
            }
            timeInfoBean.setCurrentTime(currentTime);
            timeInfoBean.setTotalTime(totalTime);
            onTimeInfoListener.onTimeInfo(timeInfoBean);
        }
    }

    public void onCallError(int code, String msg) {
        stop();

        if (onErrorListener != null) {
            onErrorListener.onError(code, msg);
        }
    }

    public void onCallComplete() {
        stop();

        if (onCompleteListener != null) {
            onCompleteListener.onComplete();
        }
    }

    public void onCallNext() {
        if (playNext) {
            playNext = false;
            prepared();
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

    public void seek(int secds) {
        _seek(secds);
    }

    public void stop() {
        timeInfoBean = null;
        duration = -1;
        new Thread(new Runnable() {
            @Override
            public void run() {
                _stop();
            }
        }).start();
    }

    public void playNext(String url) {
        source = url;
        playNext = true;
        stop();
    }

    public int getDuration() {
        if (duration < 0) {
            duration = _duration();
        }
        return duration;
    }

    public void setVolume(int percent) {
        if ((percent >= 0) && (percent <= 100)) {
            volumePercent = percent;
            _volume(percent);
        }
    }

    public int getVolumePercent() {
        return volumePercent;
    }

    public void setMute(MuteEnum mute) {
        muteEnum = mute;
        _mute(mute.getValue());
    }

    public void setPitch(float p){
        pitch = p;
        _pitch(pitch);
    }

    public void setSpeed(float s){
        speed = s;
        _speed(speed);
    }

    private native void _prepared(String source);

    private native void _start();

    private native void _pause();

    private native void _resume();

    private native void _stop();

    private native void _seek(int secds);

    private native int _duration();

    private native void _volume(int percent);

    private native void _mute(int mute);

    private native void _pitch(float pitch);

    private native void _speed(float speed);
}
