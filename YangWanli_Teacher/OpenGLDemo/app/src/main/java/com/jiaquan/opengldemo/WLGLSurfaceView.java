package com.jiaquan.opengldemo;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class WLGLSurfaceView extends GLSurfaceView {
    public WLGLSurfaceView(Context context) {
        this(context, null);
    }

    public WLGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        setRenderer(new WLRender(context));
    }
}
