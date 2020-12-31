package com.jiaquan.nativeopengldemo;

import android.Manifest;
import android.os.Build;
import android.os.Build.VERSION_CODES;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.opengl.NativeOpengl;
import com.jiaquan.opengl.WlSurfaceView;

import java.io.File;
import java.io.FileInputStream;

@RequiresApi(api = VERSION_CODES.M)
public class YUVPlayer extends AppCompatActivity {
    private WlSurfaceView wlSurfaceView = null;
    private NativeOpengl nativeOpengl = null;

    private boolean isExit = false;

    private FileInputStream fis = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_yuvplayer);

        // 要申请的权限
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE
                , Manifest.permission.ACCESS_NETWORK_STATE, Manifest.permission.CHANGE_NETWORK_STATE};

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(permissions, 321);
        }

        wlSurfaceView = findViewById(R.id.wlSurfaceview);
        nativeOpengl = new NativeOpengl();
        wlSurfaceView.setNativeOpengl(nativeOpengl);
    }

    public void play(View view) {
        if (!isExit) {
            isExit = false;

            new Thread(new Runnable() {
                @Override
                public void run() {
                    int w = 720;
                    int h = 1280;

                    try {
                        fis = new FileInputStream(new File("/sdcard/testziliao/biterate9.yuv"));

                        byte[] y = new byte[w * h];
                        byte[] u = new byte[w * h / 4];
                        byte[] v = new byte[w * h / 4];

                        while (true) {
                            if (isExit) {
                                break;
                            }

                            int ysize = fis.read(y);
                            int usize = fis.read(u);
                            int vsize = fis.read(v);
                            if ((ysize > 0) && (usize > 0) && (vsize > 0)) {
                                nativeOpengl.setYuvData(y, u, v, w, h);
                                Thread.sleep(40);
                            } else {
                                isExit = true;
                            }
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }).start();
        }
    }

    public void stop(View view) {
        isExit = true;
    }
}
