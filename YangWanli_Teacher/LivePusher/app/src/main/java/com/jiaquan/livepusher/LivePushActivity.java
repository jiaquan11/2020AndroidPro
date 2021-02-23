package com.jiaquan.livepusher;

import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.livepusher.camera.WLCameraView;
import com.jiaquan.livepusher.push.WLBasePushEncoder;
import com.jiaquan.livepusher.push.WLConnectListener;
import com.jiaquan.livepusher.push.WLPushEncoder;
import com.jiaquan.livepusher.push.WLPushVideo;

public class LivePushActivity extends AppCompatActivity {
    private WLPushVideo wlPushVideo;
    private WLCameraView wlCameraView;
    private boolean start = false;
    private WLPushEncoder wlPushEncoder;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_livepush);

        wlCameraView = findViewById(R.id.cameraview);

        wlPushVideo = new WLPushVideo();
        wlPushVideo.setWlConnectListener(new WLConnectListener() {
            @Override
            public void onConnecting() {
                Log.i("LivePushActivity","开始连接服务器中!!!");
            }

            @Override
            public void onConnectSuccess() {
                Log.i("LivePushActivity","连接服务器成功，可以开始推流!!!");

                wlPushEncoder = new WLPushEncoder(LivePushActivity.this, wlCameraView.getTextureId());
                wlPushEncoder.initEncoder(wlCameraView.getEglContext(), 720/2, 1280/2);
                wlPushEncoder.startRecord();

                wlPushEncoder.setOnMediaInfoListener(new WLBasePushEncoder.OnMediaInfoListener() {
                    @Override
                    public void onMediaTime(int times) {

                    }

                    @Override
                    public void onSPSPPSInfo(byte[] sps, byte[] pps) {
                        wlPushVideo.pushSPSPPS(sps, pps);
                    }

                    @Override
                    public void onVideoInfo(byte[] data, boolean keyframe) {
                        wlPushVideo.pushVideoData(data, keyframe);
                    }

                    @Override
                    public void onAudioInfo(byte[] data) {
                        wlPushVideo.pushAudioData(data);
                    }
                });
            }

            @Override
            public void onConnectFail(String msg) {
                Log.e("LivePushActivity", msg);
            }
        });
    }

    public void startPush(View view) {
        Log.i("LivePushActivity","startPush in");
        start = !start;

        Log.i("LivePushActivity","startPush status: " + start);
        if (start){
            wlPushVideo.initLivePush("rtmp://192.168.0.108/myapp/mystream");
        }else{
            if (wlPushEncoder != null){
                Log.i("LivePushActivity","call stopRecord");
                wlPushEncoder.stopRecord();
                wlPushEncoder = null;

                Log.i("LivePushActivity","call stopPush");
                wlPushVideo.stopPush();
            }
        }

        Log.i("LivePushActivity","startPush out");
    }
}
