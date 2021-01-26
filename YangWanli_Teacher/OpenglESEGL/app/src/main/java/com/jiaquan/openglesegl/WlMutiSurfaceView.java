package com.jiaquan.openglesegl;

import android.content.Context;
import android.util.AttributeSet;

public class WlMutiSurfaceView extends WLEGLSurfaceView{
    private WLMutiRender wlMutiRender = null;

    public WlMutiSurfaceView(Context context) {
        this(context, null);
    }

    public WlMutiSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WlMutiSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        wlMutiRender = new WLMutiRender(context);
        setRender(wlMutiRender);
    }

    public void setTextureId(int textureId, int index){
        if (wlMutiRender != null){
            wlMutiRender.setTextureId(textureId, index);
        }
    }
}
