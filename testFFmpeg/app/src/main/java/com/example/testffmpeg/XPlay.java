package com.example.testffmpeg;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;

public class XPlay extends GLSurfaceView implements Runnable, SurfaceHolder.Callback {

    public XPlay(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void run() {
        //Open("/sdcard/testziliao/biterate9.mp4", getHolder().getSurface());
        Open("/storage/emulated/0/Pictures/biterate9.mp4", getHolder().getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder){
        new Thread(this).start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                               int height){

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder){

    }

    public native void Open(String url, Object surface);
}
