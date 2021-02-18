package com.jiaquan.livepusher;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.livepusher.push.WLConnectListener;
import com.jiaquan.livepusher.push.WLPushVideo;

public class LivePushActivity extends AppCompatActivity {
    private WLPushVideo wlPushVideo;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_livepush);

        wlPushVideo = new WLPushVideo();
        wlPushVideo.setWlConnectListener(new WLConnectListener() {
            @Override
            public void onConnecting() {
                Log.i("LivePushActivity","开始连接服务器中!!!");
            }

            @Override
            public void onConnectSuccess() {
                Log.i("LivePushActivity","连接服务器成功，可以开始推流!!!");
            }

            @Override
            public void onConnectFail(String msg) {
                Log.e("LivePushActivity", msg);
            }
        });
    }

    public void startPush(View view) {
        wlPushVideo.initLivePush("rtmp://192.168.0.108/myapp/mystream");
    }
}
