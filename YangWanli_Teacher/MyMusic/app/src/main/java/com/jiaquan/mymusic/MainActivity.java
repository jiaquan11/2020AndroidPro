package com.jiaquan.mymusic;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.log.MyLog;
import com.jiaquan.myplayer.player.WLPlayer;

public class MainActivity extends AppCompatActivity {
    private WLPlayer wlPlayer = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        wlPlayer = new WLPlayer();
        wlPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                MyLog.i("onPrepared");
                wlPlayer.start();
            }
        });
    }

    public void begin(View view) {
        wlPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        wlPlayer.prepared();
    }
}