package com.jiaquan.livepusher.yuv;

import android.content.Context;
import android.util.AttributeSet;

import com.jiaquan.livepusher.egl.WLEGLSurfaceView;

public class WLYuvView extends WLEGLSurfaceView {
    private WLYuvRender wlYuvRender;

    public WLYuvView(Context context) {
        this(context, null);
    }

    public WLYuvView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WLYuvView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        wlYuvRender = new WLYuvRender(context);
        setRender(wlYuvRender);
        setRenderMode(WLEGLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void setFrameData(int w, int h, byte[] by, byte[] bu, byte[] bv) {
        if (wlYuvRender != null) {
            wlYuvRender.setFrameData(w, h, by, bu, bv);
            requestRender();
        }
    }
}
