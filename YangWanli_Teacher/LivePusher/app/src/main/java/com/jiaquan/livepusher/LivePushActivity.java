package com.jiaquan.livepusher;

import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.livepusher.push.WLPushVideo;

public class LivePushActivity extends AppCompatActivity {
    private WLPushVideo wlPushVideo;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_livepush);

        wlPushVideo = new WLPushVideo();
    }

    public void startPush(View view) {
        wlPushVideo.initLivePush("rtmp://192.168.0.108/myapp/mystream");
    }
}
