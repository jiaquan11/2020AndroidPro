package com.jiaquan.livepusher.camera;

import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;

import com.jiaquan.livepusher.util.DisplayUtil;

import java.io.IOException;
import java.util.List;

public class WLCamera {
    private SurfaceTexture surfaceTexture;
    private Camera camera;

    private int width;
    private int height;

    public WLCamera(Context context) {
        this.width = DisplayUtil.getScreenWidth(context);
        this.height = DisplayUtil.getScreenHeight(context);
        Log.i("WLCamera", "WLCamera width: " + width + " height: " + height);
    }

    public void initCamera(SurfaceTexture surfaceTexture, int cameraId) {
        this.surfaceTexture = surfaceTexture;

        setCameraParam(cameraId);
    }

    private void setCameraParam(int cameraId) {
        try {
            camera = Camera.open(cameraId);
            camera.setPreviewTexture(surfaceTexture);

            Camera.Parameters parameters = camera.getParameters();
            parameters.setFlashMode("off");
            parameters.setPreviewFormat(ImageFormat.NV21);

            Camera.Size size = getFitSize(parameters.getSupportedPictureSizes());
            parameters.setPictureSize(size.width, size.height);
            Log.i("WLCamera", "setPictureSize width: " + size.width + " height: " + size.height);
            size = getFitSize(parameters.getSupportedPreviewSizes());
            parameters.setPreviewSize(size.width, size.height);
            Log.i("WLCamera", "setPreviewSize width: " + size.width + " height: " + size.height);

            camera.setParameters(parameters);
            camera.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void stopPreview() {
        if (camera != null) {
            camera.stopPreview();
            camera.release();
            camera = null;
        }
    }

    public void changeCamera(int cameraId) {
        if (camera != null) {
            stopPreview();
        }

        setCameraParam(cameraId);
    }

    private Camera.Size getFitSize(List<Camera.Size> sizes) {
        if (width < height) {
            int t = height;
            height = width;
            width = t;
        }

        for (Camera.Size size : sizes) {
            if ((1.0f * size.width / size.height) == (1.0f * width / height)) {
                return size;
            }
        }

        Log.i("WLCamera", "getFitSize sizes.get(0) width: " + sizes.get(0).width + " height: " + sizes.get(0).height);
        return sizes.get(0);
    }
}
