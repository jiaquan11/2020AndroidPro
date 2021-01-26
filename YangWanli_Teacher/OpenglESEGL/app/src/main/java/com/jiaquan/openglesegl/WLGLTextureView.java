package com.jiaquan.openglesegl;

import android.content.Context;
import android.util.AttributeSet;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class WLGLTextureView extends WLEGLSurfaceView{
    private WLTextureRender wlTextureRender;

    public WLGLTextureView(Context context) {
        this(context, null);
    }

    public WLGLTextureView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WLGLTextureView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        wlTextureRender = new WLTextureRender(context);
        setRender(wlTextureRender);
    }

    public WLTextureRender getWlTextureRender(){
        return wlTextureRender;
    }
}
