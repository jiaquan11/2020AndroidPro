package com.jiaquan.mymusic;

import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.myplayer.TimeInfoBean;
import com.jiaquan.myplayer.listener.OnPcmInfoListener;
import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.listener.OnTimeInfoListener;
import com.jiaquan.myplayer.log.MyLog;
import com.jiaquan.myplayer.player.WLPlayer;

public class CutActivity extends AppCompatActivity {

    private WLPlayer wlPlayer = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cutaudio);

        wlPlayer = new WLPlayer();

        wlPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                wlPlayer.cutAudioPlay(20, 40, true);
            }
        });

        wlPlayer.setOnTimeInfoListener(new OnTimeInfoListener() {
            @Override
            public void onTimeInfo(TimeInfoBean timeInfoBean) {
                MyLog.i(timeInfoBean.toString());
            }
        });

        wlPlayer.setOnPcmInfoListener(new OnPcmInfoListener() {
            @Override
            public void onPcmInfo(byte[] buffer, int buffersize) {
                MyLog.i("PcmInfo bufferSize: " + buffersize);
            }

            @Override
            public void onPcmRate(int samplerate, int bit, int channels) {
                MyLog.i("PcmInfo samplerate: " + samplerate + " bit:" + bit + " channels:" + channels);
            }
        });
    }

    public void cutAudio(View view) {
//        wlPlayer.setSource("/sdcard/testziliao/first-love-wangxinling.ape");
        wlPlayer.setSource("/sdcard/testziliao/mydream.m4a");
        wlPlayer.prepared();
    }
}
