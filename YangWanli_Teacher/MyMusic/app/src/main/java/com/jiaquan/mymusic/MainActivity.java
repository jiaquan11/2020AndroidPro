package com.jiaquan.mymusic;

import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.myplayer.TimeInfoBean;
import com.jiaquan.myplayer.listener.OnCompleteListener;
import com.jiaquan.myplayer.listener.OnErrorListener;
import com.jiaquan.myplayer.listener.OnLoadListener;
import com.jiaquan.myplayer.listener.OnPauseResumeListener;
import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.listener.OnRecordTimeListener;
import com.jiaquan.myplayer.listener.OnTimeInfoListener;
import com.jiaquan.myplayer.listener.OnVolumeDBListener;
import com.jiaquan.myplayer.log.MyLog;
import com.jiaquan.myplayer.muteenum.MuteEnum;
import com.jiaquan.myplayer.opengl.WLGLSurfaceView;
import com.jiaquan.myplayer.player.WLPlayer;
import com.jiaquan.myplayer.util.TimeUtil;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    private WLPlayer wlPlayer = null;
    private TextView tv_time = null;
    private TextView tv_volume = null;
    private SeekBar seekBarSeek = null;
    private int position = 0;
    private boolean isSeekBar = false;

    private SeekBar seekBarVolume = null;

    private WLGLSurfaceView wlglSurfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        wlglSurfaceView = findViewById(R.id.wlglsurfaceview);

        tv_time = findViewById(R.id.tv_time);
        seekBarSeek = findViewById(R.id.seekbar_seek);
        seekBarVolume = findViewById(R.id.seekbar_volume);
        tv_volume = findViewById(R.id.tv_volume);

        // 要申请的权限
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE
                , Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.CHANGE_NETWORK_STATE};

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 321);
        }

        wlPlayer = new WLPlayer();
        wlPlayer.setWlglSurfaceView(wlglSurfaceView);

        wlPlayer.setVolume(80);//设置初始音量
        wlPlayer.setMute(MuteEnum.MUTE_LEFT);
        tv_volume.setText("音量: " + wlPlayer.getVolumePercent() + "%");
        wlPlayer.setPitch(1.0f);
        wlPlayer.setSpeed(1.0f);
        seekBarVolume.setProgress(wlPlayer.getVolumePercent());

        wlPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                MyLog.i("onPrepared");
                wlPlayer.start();
            }
        });

        wlPlayer.setOnLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean load) {
                if (load) {
                    MyLog.i("加载中");
                } else {
                    MyLog.i("播放中");
                }
            }
        });

        wlPlayer.setOnPauseResumeListener(new OnPauseResumeListener() {
            @Override
            public void onPause(boolean pause) {
                if (pause) {
                    MyLog.i("暂停中");
                } else {
                    MyLog.i("恢复播放中");
                }
            }
        });

        wlPlayer.setOnTimeInfoListener(new OnTimeInfoListener() {
            @Override
            public void onTimeInfo(TimeInfoBean timeInfoBean) {
//                MyLog.i(timeInfoBean.toString());
                Message message = Message.obtain();
                message.what = 1;
                message.obj = timeInfoBean;
                handler.sendMessage(message);
            }
        });

        wlPlayer.setOnErrorListener(new OnErrorListener() {
            @Override
            public void onError(int code, String msg) {
                MyLog.i("code is: " + code + " msg: " + msg);
            }
        });

        wlPlayer.setOnCompleteListener(new OnCompleteListener() {
            @Override
            public void onComplete() {
                MyLog.i("播放完成了");
            }
        });

        wlPlayer.setOnVolumeDBListener(new OnVolumeDBListener() {
            @Override
            public void onDBValue(int db) {
//                MyLog.i("db is " + db);
            }
        });

        wlPlayer.setOnRecordTimeListener(new OnRecordTimeListener() {
            @Override
            public void onRecordTime(int recordTime) {
//                MyLog.i("record time is: " + recordTime);
            }
        });

        seekBarSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (wlPlayer.getDuration() > 0 && isSeekBar) {
                    position = wlPlayer.getDuration() * progress / 100;
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                isSeekBar = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                wlPlayer.seek(position);
                isSeekBar = false;
            }
        });

        seekBarVolume.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                wlPlayer.setVolume(progress);
                tv_volume.setText("音量: " + wlPlayer.getVolumePercent() + "%");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    public void begin(View view) {
//        wlPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
//        wlPlayer.setSource("/sdcard/testziliao/mydream.m4a");
//        wlPlayer.setSource("/sdcard/testziliao/fcrs.1080p.mp4");
//        wlPlayer.setSource("/sdcard/testziliao/biterate9.mp4");
//        wlPlayer.setSource("/sdcard/testziliao/biterate9noB.mp4");
        wlPlayer.setSource("/sdcard/testziliao/chuqiaozhuan1.mp4");

//        wlPlayer.setSource("/sdcard/testziliao/first-love-wangxinling.ape");
//        wlPlayer.setSource("http://ngcdn004.cnr.cn/live/dszs/index.m3u8");
//        wlPlayer.setSource("/sdcard/testziliao/yongqi-liangjingru.m4a");

        wlPlayer.prepared();
    }

    public void pause(View view) {
        wlPlayer.pause();
    }

    public void resume(View view) {
        wlPlayer.resume();
    }

    Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {//重写类方法
            super.handleMessage(msg);

            if (msg.what == 1) {
                if (!isSeekBar) {
                    TimeInfoBean timeInfoBean = (TimeInfoBean) msg.obj;
                    tv_time.setText(TimeUtil.secdsToDateFormat(timeInfoBean.getTotalTime(), timeInfoBean.getTotalTime())
                            + "/" + TimeUtil.secdsToDateFormat(timeInfoBean.getCurrentTime(), timeInfoBean.getTotalTime()));

                    seekBarSeek.setProgress(timeInfoBean.getCurrentTime() * 100 / timeInfoBean.getTotalTime());
                }
            }
        }
    };

    public void stop(View view) {
        wlPlayer.stop();
    }

    public void seek(View view) {
        wlPlayer.seek(20);
    }

    public void next(View view) {
//        wlPlayer.playNext("http://ngcdn004.cnr.cn/live/dszs/index.m3u8");
//        wlPlayer.playNext("/sdcard/testziliao/first-love-wangxinling.ape");
//        wlPlayer.playNext("/sdcard/testziliao/fcrs.1080p.mp4");
        wlPlayer.playNext("/sdcard/testziliao/建国大业.mpg");
    }

    public void left(View view) {
        wlPlayer.setMute(MuteEnum.MUTE_LEFT);
    }

    public void right(View view) {
        wlPlayer.setMute(MuteEnum.MUTE_RIGHT);
    }

    public void center(View view) {
        wlPlayer.setMute(MuteEnum.MUTE_CENTER);
    }

    //变速不变调
    public void speed(View view) {
        wlPlayer.setPitch(1.0f);
        wlPlayer.setSpeed(1.5f);
    }

    //变调不变速
    public void pitch(View view) {
        wlPlayer.setPitch(1.5f);
        wlPlayer.setSpeed(1.0f);
    }

    public void speedpitch(View view) {
        wlPlayer.setPitch(1.5f);
        wlPlayer.setSpeed(1.5f);
    }

    public void normalspeedpitch(View view) {
        wlPlayer.setPitch(1.0f);
        wlPlayer.setSpeed(1.0f);
    }

    public void start_record(View view) {
        wlPlayer.startRecord(new File("/sdcard/testziliao/testplayer1.aac"));
    }

    public void pause_record(View view) {
        wlPlayer.pauseRecord();
    }

    public void resume_record(View view) {
        wlPlayer.resumeRecord();
    }

    public void stop_record(View view) {
        wlPlayer.stopRecord();
    }
}