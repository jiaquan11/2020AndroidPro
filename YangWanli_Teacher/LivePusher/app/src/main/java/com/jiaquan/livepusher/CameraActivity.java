package com.jiaquan.livepusher;

import android.content.res.Configuration;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.livepusher.camera.WLCameraView;

public class CameraActivity extends AppCompatActivity {
    private WLCameraView wlCameraView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_camera);

        wlCameraView = findViewById(R.id.cameraview);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        wlCameraView.onDestroy();
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        wlCameraView.previewAngle(this);
    }
}
