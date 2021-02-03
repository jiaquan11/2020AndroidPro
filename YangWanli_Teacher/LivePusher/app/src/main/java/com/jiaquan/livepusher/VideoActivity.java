package com.jiaquan.livepusher;

import android.media.MediaFormat;
import android.os.Bundle;
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
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.avtivity_video);

        wlCameraView = findViewById(R.id.cameraview);
        btnRecord = findViewById(R.id.btn_record);
    }

    public void record(View view) {
        if (wlMediaEncoder == null) {
            Log.i("VideoActivity", "textureId is " + wlCameraView.getTextureId());//回调回来的FBO最终渲染的窗口纹理id

            wlMediaEncoder = new WLMediaEncoder(this, wlCameraView.getTextureId());//使用摄像头预览最终渲染到窗口的纹理进行编码
//            String destPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/testziliao/yangwlVideo.mp4";
            String destPath = "/sdcard/testziliao/yangwlVideo.mp4";
            //传入摄像头预览的eglContext给到编码器
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
