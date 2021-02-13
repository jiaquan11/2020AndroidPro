package com.jiaquan.audiorecord;

import android.Manifest;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {
    private AudioRecordUtil audioRecordUtil = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 要申请的权限
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE
                , Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.CHANGE_NETWORK_STATE, Manifest.permission.CAMERA,
                Manifest.permission.RECORD_AUDIO};

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 321);
        }
    }

    public void recordAudio(View view) {
        if (audioRecordUtil == null) {
            audioRecordUtil = new AudioRecordUtil();
            audioRecordUtil.setOnRecordListener(new AudioRecordUtil.OnRecordListener() {
                @Override
                public void recordByte(byte[] audioData, int readSize) {
                    Log.i("MainActivity", "Audio rec readSize: " + readSize);
                }
            });

            audioRecordUtil.startRecord();
        } else {
            audioRecordUtil.stopRecord();
            audioRecordUtil = null;

            Log.i("MainActivity", "stop Audio Record!");
        }
    }
}