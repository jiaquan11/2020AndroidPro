package com.jiaquan.livepusher.camera;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;

import com.jiaquan.livepusher.egl.WLEGLSurfaceView;

public class WLCameraView extends WLEGLSurfaceView {
    private WLCameraRender wlCameraRender;
    private WLCamera wlCamera;

    private int cameraId = Camera.CameraInfo.CAMERA_FACING_BACK;

    private int textureId = -1;

    public WLCameraView(Context context) {
        this(context, null);
    }

    public WLCameraView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public WLCameraView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        wlCameraRender = new WLCameraRender(context);
        wlCamera = new WLCamera(context);

        setRender(wlCameraRender);

        previewAngle(context);

        wlCameraRender.setOnSurfaceCreateListener(new WLCameraRender.OnSurfaceCreateListener() {
            @Override
            public void onSurfaceCreate(SurfaceTexture surfaceTexture, int textureid) {
                wlCamera.initCamera(surfaceTexture, cameraId);
                textureId = textureid;//回调回来的FBO最终渲染的窗口纹理id
            }
        });
    }

    public void onDestroy() {
        if (wlCamera != null) {
            wlCamera.stopPreview();
        }
    }

    public void previewAngle(Context context) {
        //获取acitvity真实的显示角度，然后调整摄像头的角度变化进行渲染
        int angle = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();

        wlCameraRender.resetMatrix();//重置矩阵，后面会进行重新赋值

        Log.i("WLCameraView", "angle is: " + angle);
        switch (angle) {
            case Surface.ROTATION_0:
                if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
                    wlCameraRender.setAngle(90, 0, 0, 1);//先旋转Z轴
                    wlCameraRender.setAngle(180, 1, 0, 0);//再旋转X轴
                } else {
                    wlCameraRender.setAngle(90, 0, 0, 1);
                }
                break;

            case Surface.ROTATION_90:
                if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
                    wlCameraRender.setAngle(180, 0, 0, 1);
                    wlCameraRender.setAngle(180, 0, 1, 0);
                } else {
                    wlCameraRender.setAngle(90, 0, 0, 1);
                }
                break;

            case Surface.ROTATION_180:
                if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
                    wlCameraRender.setAngle(90, 0, 0, 1);
                    wlCameraRender.setAngle(180, 0, 1, 0);
                } else {
                    wlCameraRender.setAngle(-90, 0, 0, 1);
                }
                break;

            case Surface.ROTATION_270:
                if (cameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
                    wlCameraRender.setAngle(180, 0, 1, 0);
                } else {
                    wlCameraRender.setAngle(0, 0, 0, 1);
                }
                break;
        }
    }

    public int getTextureId(){
        return textureId;
    }
}
