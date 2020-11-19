package com.jiaquan.nativeopengldemo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.opengl.NativeOpengl;
import com.jiaquan.opengl.WlSurfaceView;

import java.nio.ByteBuffer;

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
        wlSurfaceView.setOnSurfaceListener(new WlSurfaceView.OnSurfaceListener() {
            @Override
            public void init() {
                //解码一张bitmap图片，拿到像素数据
                final Bitmap bitmap = BitmapFactory.decodeResource(getResources(),
                        R.drawable.mingren);
                ByteBuffer fcbuffer = ByteBuffer.allocate(bitmap.getHeight() * bitmap.getWidth() * 4);
                bitmap.copyPixelsToBuffer(fcbuffer);
                fcbuffer.flip();
                byte[] pixels = fcbuffer.array();
                nativeOpengl.imgData(bitmap.getWidth(), bitmap.getHeight(), pixels.length, pixels);
            }
        });
    }

    public void changeFilter(View view) {
        if (nativeOpengl != null) {
            nativeOpengl.surfaceChangeFilter();
        }
    }
}