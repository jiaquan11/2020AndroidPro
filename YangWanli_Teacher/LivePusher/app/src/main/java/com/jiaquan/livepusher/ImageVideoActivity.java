package com.jiaquan.livepusher;

import android.os.Bundle;
import android.os.Environment;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.jiaquan.livepusher.encodec.WLMediaEncoder;
import com.jiaquan.livepusher.imgvideo.WLImgVideoView;
import com.ywl5320.libmusic.WlMusic;
import com.ywl5320.listener.OnPreparedListener;
import com.ywl5320.listener.OnShowPcmDataListener;

public class ImageVideoActivity extends AppCompatActivity {
    private WLImgVideoView wlImgVideoView;
    private WLMediaEncoder wlMediaEncoder;
    private WlMusic wlMusic;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_imagevideo);

        wlImgVideoView = findViewById(R.id.imgvideoview);
        wlImgVideoView.setCurrentImg(R.drawable.img_1);

        wlMusic = WlMusic.getInstance();
        wlMusic.setCallBackPcmData(true);

        wlMusic.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                wlMusic.playCutAudio(0, 60);
            }
        });

        wlMusic.setOnShowPcmDataListener(new OnShowPcmDataListener() {
            @Override
            public void onPcmInfo(int samplerate, int bit, int channels) {
                wlMediaEncoder = new WLMediaEncoder(ImageVideoActivity.this, wlImgVideoView.getFboTextureId());
                wlMediaEncoder.initEncoder(wlImgVideoView.getEglContext(), Environment.getExternalStorageDirectory().getAbsolutePath() + "/testziliao/image_video.mp4",
                        720, 500, samplerate, channels);

                wlMediaEncoder.startRecord();

                startImgs();
            }

            @Override
            public void onPcmData(byte[] pcmdata, int size, long clock) {
                if (wlMediaEncoder != null) {
                    wlMediaEncoder.putPCMData(pcmdata, size);
                }
            }
        });
    }

    public void start(View view) {
        wlMusic.setSource("/sdcard/testziliao/the_girl.m4a");
        wlMusic.prePared();
    }

    private void startImgs() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i = 1; i <= 257; i++) {
                    int imgsrc = getResources().getIdentifier("img_" + i, "drawable", "com.jiaquan.livepusher");
                    wlImgVideoView.setCurrentImg(imgsrc);

                    try {
                        Thread.sleep(80);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

                if (wlMediaEncoder != null) {
                    wlMusic.stop();
                    wlMediaEncoder.stopRecord();
                    wlMediaEncoder = null;
                }
            }
        }).start();
    }
}
