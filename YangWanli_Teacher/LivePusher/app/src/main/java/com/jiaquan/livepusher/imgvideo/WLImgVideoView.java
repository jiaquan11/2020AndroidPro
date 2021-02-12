package com.jiaquan.livepusher.imgvideo;

import android.content.Context;
import android.util.AttributeSet;

import com.jiaquan.livepusher.egl.WLEGLSurfaceView;

public class WLImgVideoView extends WLEGLSurfaceView {
    private WLImgVideoRender wlImgVideoRender;

    private int fboTextureId;

    public WLImgVideoView(Context context) {
        this(context, null);
    }

    public WLImgVideoView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WLImgVideoView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        wlImgVideoRender = new WLImgVideoRender(context);
        setRender(wlImgVideoRender);
        setRenderMode(WLEGLSurfaceView.RENDERMODE_WHEN_DIRTY);

        wlImgVideoRender.setOnRenderCreateListener(new WLImgVideoRender.OnRenderCreateListener() {
            @Override
            public void onCreate(int textid) {
                fboTextureId = textid;
            }
        });
    }

    public void setCurrentImg(int imgsrc) {
        if (wlImgVideoRender != null) {
            wlImgVideoRender.setCurrentImgSrc(imgsrc);

            requestRender();
        }
    }

    public int getFboTextureId() {
        return fboTextureId;
    }
}
