package com.jiaquan.nativeopengldemo;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.opengl.NativeOpengl;
import com.jiaquan.opengl.WlSurfaceView;

public class MainActivity extends AppCompatActivity {
    private WlSurfaceView wlSurfaceView = null;
    private NativeOpengl nativeOpengl = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        wlSurfaceView = findViewById(R.id.wlSurfaceview);
        nativeOpengl = new NativeOpengl();
        wlSurfaceView.setNativeOpengl(nativeOpengl);
    }
}