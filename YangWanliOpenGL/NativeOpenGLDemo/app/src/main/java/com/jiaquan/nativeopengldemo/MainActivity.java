package com.jiaquan.nativeopengldemo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;

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

        //解码一张bitmap图片，拿到像素数据
        final Bitmap bitmap = BitmapFactory.decodeResource(getResources(),
                R.drawable.testimage);
        ByteBuffer fcbuffer = ByteBuffer.allocate(bitmap.getHeight()*bitmap.getWidth()*4);
        bitmap.copyPixelsToBuffer(fcbuffer);
        fcbuffer.flip();
        byte[] pixels = fcbuffer.array();
        nativeOpengl.imgData(bitmap.getWidth(), bitmap.getHeight(), pixels.length, pixels);
    }
}