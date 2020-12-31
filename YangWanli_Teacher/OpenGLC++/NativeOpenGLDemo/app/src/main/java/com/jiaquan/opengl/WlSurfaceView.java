package com.jiaquan.opengl;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class WlSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    private NativeOpengl nativeOpengl = null;

    private OnSurfaceListener onSurfaceListener = null;

    public interface OnSurfaceListener {
        void init();
    }

    public void setOnSurfaceListener(OnSurfaceListener onSurfaceListener) {
        this.onSurfaceListener = onSurfaceListener;
    }

    public void setNativeOpengl(NativeOpengl nativeOpengl) {
        this.nativeOpengl = nativeOpengl;
    }

    public WlSurfaceView(Context context) {
        this(context, null);
    }

    public WlSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WlSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        getHolder().addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (nativeOpengl != null) {
            nativeOpengl.surfaceCreate(holder.getSurface());

//            if (onSurfaceListener != null) {
//                onSurfaceListener.init();
//            }
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (nativeOpengl != null) {
            nativeOpengl.surfaceChange(width, height);//屏幕宽高

            if (onSurfaceListener != null) {
                onSurfaceListener.init();
            }
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {//界面资源回收
        if (nativeOpengl != null) {
            nativeOpengl.surfaceDestroy();
        }
    }
}
