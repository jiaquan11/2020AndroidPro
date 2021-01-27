package com.jiaquan.livepusher.camera;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.AttributeSet;

import com.jiaquan.livepusher.egl.WLEGLSurfaceView;

public class WLCameraView extends WLEGLSurfaceView {
    private WLCameraRender wlCameraRender;
    private WLCamera wlCamera;

    private int cameraId = Camera.CameraInfo.CAMERA_FACING_BACK;

    public WLCameraView(Context context) {
        this(context, null);
    }

    public WLCameraView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WLCameraView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        wlCameraRender = new WLCameraRender(context);
        wlCamera = new WLCamera();

        setRender(wlCameraRender);

        wlCameraRender.setOnSurfaceCreateListener(new WLCameraRender.OnSurfaceCreateListener() {
            @Override
            public void onSurfaceCreate(SurfaceTexture surfaceTexture) {
                wlCamera.initCamera(surfaceTexture, cameraId);
            }
        });
    }

    public void onDestroy(){
        if (wlCamera != null){
            wlCamera.stopPreview();
        }
    }
}
