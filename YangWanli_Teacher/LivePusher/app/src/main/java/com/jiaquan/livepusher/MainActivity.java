package com.jiaquan.livepusher;

import android.Manifest;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 要申请的权限
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE
                , Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.CHANGE_NETWORK_STATE, Manifest.permission.CAMERA,
        Manifest.permission.INTERNET, Manifest.permission.RECORD_AUDIO};

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 321);
        }
    }

    public void cameraPreview(View view) {
        Intent intent = new Intent(this, CameraActivity.class);
        startActivity(intent);
    }

    public void videoRecord(View view) {
        Intent intent = new Intent(this, VideoActivity.class);
        startActivity(intent);
    }

    public void imgVideo(View view) {
        Intent intent = new Intent(this, ImageVideoActivity.class);
        startActivity(intent);
    }

    public void yuvPlayer(View view) {
        Intent intent = new Intent(this, YuvActivity.class);
        startActivity(intent);
    }

    public void livepush(View view) {
        Intent intent = new Intent(this, LivePushActivity.class);
        startActivity(intent);
    }
}