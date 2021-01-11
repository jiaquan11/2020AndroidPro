package com.jiaquan.myplayer.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.jiaquan.myplayer.log.MyLog;

public class WLGLSurfaceView extends GLSurfaceView {
    private WLRender wlRender;

    public WLGLSurfaceView(Context context) {
        this(context, null);
    }

    public WLGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        MyLog.i("construct WLGLSurfaceView in");

        setEGLContextClientVersion(2);
        wlRender = new WLRender(context);
        setRenderer(wlRender);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);//手动渲染

        //用于硬解
        wlRender.setOnRenderListener(new WLRender.OnRenderListener() {
            @Override
            public void onRender() {
                requestRender();
            }
        });
        MyLog.i("construct WLGLSurfaceView end");
    }

    public void setYUVData(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (wlRender != null) {
            wlRender.setYUVRenderData(width, height, y, u, v);
            requestRender();
        }
    }

    public WLRender getWlRender() {
        return wlRender;
    }
}
