package com.jiaquan.nativeopengldemo;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.opengl.NativeOpengl;
import com.jiaquan.opengl.WlSurfaceView;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private WlSurfaceView wlSurfaceView = null;
    private NativeOpengl nativeOpengl = null;

    private byte[] pixels;
    private List<Integer> imgList = new ArrayList<>();
    private int index = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        wlSurfaceView = findViewById(R.id.wlSurfaceview);
        nativeOpengl = new NativeOpengl();
        wlSurfaceView.setNativeOpengl(nativeOpengl);

        imgList.add(R.drawable.mingren);
        imgList.add(R.drawable.img_1);
        imgList.add(R.drawable.img_2);
        imgList.add(R.drawable.img_3);

        wlSurfaceView.setOnSurfaceListener(new WlSurfaceView.OnSurfaceListener() {
            @Override
            public void init() {
                readPixels();
            }
        });
    }

    public void changeFilter(View view) {
        if (nativeOpengl != null) {
            nativeOpengl.surfaceChangeFilter();
        }
    }

    public void changeTexture(View view) {
        readPixels();
    }

    private void readPixels(){
        //解码一张bitmap图片，拿到像素数据
        final Bitmap bitmap = BitmapFactory.decodeResource(getResources(),
                getImageIds());
        ByteBuffer fcbuffer = ByteBuffer.allocate(bitmap.getHeight() * bitmap.getWidth() * 4);
        bitmap.copyPixelsToBuffer(fcbuffer);
        fcbuffer.flip();
        pixels = fcbuffer.array();
        nativeOpengl.imgData(bitmap.getWidth(), bitmap.getHeight(), pixels.length, pixels);
    }

    private int getImageIds(){
        index++;
        if (index >= imgList.size()){
            index = 0;
        }
        return imgList.get(index);
    }
}