package com.jiaquan.openglesegl;

import android.content.Context;
import android.util.AttributeSet;

public class WLGLSurfaceView extends WLEGLSurfaceView {
    public WLGLSurfaceView(Context context) {
        this(context, null);
    }

    public WLGLSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WLGLSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        setRender(new WLRender());
        setRenderMode(WLEGLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }
}
