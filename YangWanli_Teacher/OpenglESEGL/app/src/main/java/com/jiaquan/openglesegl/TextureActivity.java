package com.jiaquan.openglesegl;

import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup;
import android.widget.LinearLayout;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

public class TextureActivity extends AppCompatActivity {
    private WLGLTextureView wlglTextureView;
    private LinearLayout lyContent;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_texture);

        wlglTextureView = findViewById(R.id.wlglsurfaceview);
        lyContent = findViewById(R.id.ly_content);

        wlglTextureView.getWlTextureRender().setOnRenderCreateListener(new WLTextureRender.OnRenderCreateListener() {
            @Override
            public void onCreate(final int textid) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (lyContent.getChildCount() > 0) {
                            lyContent.removeAllViews();
                        }

                        for (int i = 0; i < 3; i++) {
                            WlMutiSurfaceView wlMutiSurfaceView = new WlMutiSurfaceView(TextureActivity.this);

                            wlMutiSurfaceView.setTextureId(textid, i);
                            wlMutiSurfaceView.setSurfaceAndEglContext(null, wlglTextureView.getEglContext());

                            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
                            lp.width = 200;
                            lp.height = 300;

                            wlMutiSurfaceView.setLayoutParams(lp);

                            lyContent.addView(wlMutiSurfaceView);
                        }
                    }
                });
            }
        });
    }
}
