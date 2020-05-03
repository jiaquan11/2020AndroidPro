package com.jiaquan.xplay;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class XPlay extends GLSurfaceView implements SurfaceHolder.Callback, GLSurfaceView.Renderer{
    public XPlay(Context context, AttributeSet attrs) {
        super(context, attrs);

        //android 8.0需要设置
        setRenderer(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder){
        Log.i("XPlay", "surfaceCreated");
        //初始化opengl egl 显示
        InitView(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                               int height){

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder){

    }

    public native void InitView(Object surface);

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {

    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {

    }

    @Override
    public void onDrawFrame(GL10 gl10) {

    }
}
