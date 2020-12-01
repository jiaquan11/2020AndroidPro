package com.jiaquan.mymusic;

import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.myplayer.listener.OnLoadListener;
import com.jiaquan.myplayer.listener.OnPauseResumeListener;
import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.log.MyLog;
import com.jiaquan.myplayer.player.WLPlayer;

public class MainActivity extends AppCompatActivity {
    private WLPlayer wlPlayer = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 要申请的权限
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE
                , Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.CHANGE_NETWORK_STATE};

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 321);
        }

        wlPlayer = new WLPlayer();
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
    }

    public void begin(View view) {
//        wlPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        wlPlayer.setSource("/sdcard/testziliao/mydream.m4a");
        wlPlayer.prepared();
    }

    public void pause(View view) {
        wlPlayer.pause();
    }

    public void resume(View view) {
        wlPlayer.resume();
    }
}