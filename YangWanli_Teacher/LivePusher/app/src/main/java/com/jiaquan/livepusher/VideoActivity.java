package com.jiaquan.livepusher;

import android.Manifest;
import android.media.MediaFormat;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.PersistableBundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.livepusher.camera.WLCameraView;
import com.jiaquan.livepusher.encodec.WLBaseMediaEncoder;
import com.jiaquan.livepusher.encodec.WLMediaEncoder;

public class VideoActivity extends AppCompatActivity {
    private WLCameraView wlCameraView;
    private Button btnRecord;

    private WLMediaEncoder wlMediaEncoder;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState, @Nullable PersistableBundle persistentState) {
        super.onCreate(savedInstanceState, persistentState);

        // 要申请的权限
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE
                , Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.CHANGE_NETWORK_STATE};

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 321);
        }

        setContentView(R.layout.avtivity_video);
        wlCameraView = findViewById(R.id.cameraview);
        btnRecord = findViewById(R.id.btn_record);
    }

    public void record(View view) {
        if (wlMediaEncoder == null) {
            Log.i("VideoActivity", "textureId is " + wlCameraView.getTextureId());

            wlMediaEncoder = new WLMediaEncoder(this, wlCameraView.getTextureId());
//            String destPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/testziliao/yangwlVideo.mp4";
            String destPath = "/sdcard/testziliao/yangwlVideo.mp4";
            wlMediaEncoder.initEncoder(wlCameraView.getEglContext(), destPath, MediaFormat.MIMETYPE_VIDEO_AVC, 720, 1280);
            wlMediaEncoder.setOnMediaInfoListener(new WLBaseMediaEncoder.OnMediaInfoListener() {
                @Override
                public void onMediaTime(int times) {
                    Log.i("VideoActivity", "rec time is: " + times);
                }
            });

            wlMediaEncoder.startRecord();

            btnRecord.setText("正在录制");
        } else {
            wlMediaEncoder.stopRecord();
            btnRecord.setText("开始录制");

            wlMediaEncoder = null;
        }
    }
}
