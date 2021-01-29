package com.jiaquan.livepusher.encodec;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;
import android.view.Surface;

import com.jiaquan.livepusher.egl.EglHelper;
import com.jiaquan.livepusher.egl.WLEGLSurfaceView;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLContext;

public abstract class WLBaseMediaEncoder {
    private Surface surface;
    private EGLContext eglContext;

    private int width;
    private int height;

    private MediaCodec videoEncoder;
    private MediaFormat videoFormat;
    private MediaCodec.BufferInfo videoBufferInfo;

    private MediaMuxer mediaMuxer;

    private WLEGLMediaThread wleglMediaThread;
    private VideoEncoderThread videoEncoderThread;

    private WLEGLSurfaceView.WLGLRender wlglRender = null;

    private int mRenderMode = RENDERMODE_CONTINUOUSLY;

    public final static int RENDERMODE_WHEN_DIRTY = 0;
    public final static int RENDERMODE_CONTINUOUSLY = 1;

    private OnMediaInfoListener onMediaInfoListener;

    public void setOnMediaInfoListener(OnMediaInfoListener onMediaInfoListener) {
        this.onMediaInfoListener = onMediaInfoListener;
    }

    public WLBaseMediaEncoder(Context context) {

    }

    public void setRender(WLEGLSurfaceView.WLGLRender wlglRender) {
        this.wlglRender = wlglRender;
    }

    public void setmRenderMode(int mRenderMode) {
        if (wlglRender == null) {
            throw new RuntimeException("must set render before");
        }

        this.mRenderMode = mRenderMode;
    }

    public void initEncoder(EGLContext eglContext, String savePath, String mimeType, int width, int height) {
        this.width = width;
        this.height = height;
        this.eglContext = eglContext;
        initMediaEncoder(savePath, mimeType, width, height);
    }

    public void startRecord() {
        if ((surface != null) && (eglContext != null)) {
            wleglMediaThread = new WLEGLMediaThread(new WeakReference<WLBaseMediaEncoder>(this));
            videoEncoderThread = new VideoEncoderThread(new WeakReference<WLBaseMediaEncoder>(this));
            wleglMediaThread.isCreate = true;
            wleglMediaThread.isChange = true;
            wleglMediaThread.start();
            videoEncoderThread.start();
        }
    }

    public void stopRecord() {
        if ((wleglMediaThread != null) && (videoEncoderThread != null)) {
            videoEncoderThread.exit();
            wleglMediaThread.onDestroy();
            videoEncoderThread = null;
            wleglMediaThread = null;
        }
    }

    private void initMediaEncoder(String savePath, String mimeType, int width, int height) {
        try {
            mediaMuxer = new MediaMuxer(savePath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            initVideoEncoder(mimeType, width, height);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void initVideoEncoder(String mimeType, int width, int height) {
        try {
            videoBufferInfo = new MediaCodec.BufferInfo();
            videoFormat = MediaFormat.createVideoFormat(mimeType, width, height);
            videoFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
            videoFormat.setInteger(MediaFormat.KEY_BIT_RATE, width * height * 4);
            videoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 30);
            videoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1);

            videoEncoder = MediaCodec.createEncoderByType(mimeType);
            videoEncoder.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);

            surface = videoEncoder.createInputSurface();
        } catch (IOException e) {
            e.printStackTrace();
            videoEncoder = null;
            videoFormat = null;
            videoBufferInfo = null;
        }
    }

    static class WLEGLMediaThread extends Thread {
        private WeakReference<WLBaseMediaEncoder> encoder;
        private EglHelper eglHelper;
        private Object object;

        private boolean isExit = false;
        private boolean isCreate = false;
        private boolean isChange = false;
        private boolean isStart = false;

        public WLEGLMediaThread(WeakReference<WLBaseMediaEncoder> encoder) {
            this.encoder = encoder;
        }

        @Override
        public void run() {
            super.run();

            isExit = false;
            isStart = false;
            object = new Object();
            eglHelper = new EglHelper();
            eglHelper.initEgl(encoder.get().surface, encoder.get().eglContext);

            while (true) {
                if (isExit) {
                    release();
                    break;
                }

                if (isStart) {
                    if (encoder.get().mRenderMode == RENDERMODE_WHEN_DIRTY) {
                        synchronized (object) {
                            try {
                                object.wait();
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    } else if (encoder.get().mRenderMode == RENDERMODE_CONTINUOUSLY) {
                        try {
                            Thread.sleep(1000 / 60);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    } else {
                        throw new RuntimeException("mRenderMode is wrong value");
                    }
                }

                onCreate();
                onChange(encoder.get().width, encoder.get().height);
                onDraw();

                isStart = true;
            }
        }

        private void onCreate() {
            if (isCreate && encoder.get().wlglRender != null) {
                isCreate = false;
                encoder.get().wlglRender.onSurfaceCreated();
            }
        }

        private void onChange(int width, int height) {
            if (isChange && encoder.get().wlglRender != null) {
                isChange = false;
                encoder.get().wlglRender.onSurfaceChanged(width, height);
            }
        }

        private void onDraw() {
            if ((encoder.get().wlglRender != null) && (eglHelper != null)) {
                encoder.get().wlglRender.onDrawFrame();

//                if (!isStart) {
//                    encoder.get().wlglRender.onDrawFrame();
//                }

                eglHelper.swapBuffers();
            }
        }

        private void requestRender() {
            if (object != null) {
                synchronized (object) {
                    object.notifyAll();
                }
            }
        }

        public void onDestroy() {
            isExit = true;
            requestRender();
        }

        public void release() {
            if (eglHelper != null) {
                eglHelper.destroyEgl();
                eglHelper = null;
                object = null;
                encoder = null;
            }
        }
    }

    static class VideoEncoderThread extends Thread {
        private WeakReference<WLBaseMediaEncoder> encoder;

        private boolean isExit;
        private MediaCodec videoEncoder;
        private MediaFormat videoFormat;
        private MediaCodec.BufferInfo videoBufferInfo;
        private MediaMuxer mediaMuxer;
        private long pts;

        private int videoTrackIndex;

        VideoEncoderThread(WeakReference<WLBaseMediaEncoder> encoder) {
            this.encoder = encoder;
            videoEncoder = encoder.get().videoEncoder;
            videoFormat = encoder.get().videoFormat;
            videoBufferInfo = encoder.get().videoBufferInfo;
            mediaMuxer = encoder.get().mediaMuxer;
        }

        @Override
        public void run() {
            super.run();

            pts = 0;
            videoTrackIndex = -1;
            isExit = false;

            videoEncoder.start();

            while (true) {
                if (isExit) {
                    videoEncoder.stop();
                    videoEncoder.release();
                    videoEncoder = null;

                    mediaMuxer.stop();
                    mediaMuxer.release();
                    mediaMuxer = null;
                    Log.i("WLBaseMediaEncoder", "录制完成!!!");
                    break;
                }

                int outputBufferIndex = videoEncoder.dequeueOutputBuffer(videoBufferInfo, 0);
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    videoTrackIndex = mediaMuxer.addTrack(videoEncoder.getOutputFormat());
                    mediaMuxer.start();
                } else {
                    while (outputBufferIndex >= 0) {
                        ByteBuffer outputBuffer = videoEncoder.getOutputBuffers()[outputBufferIndex];
                        outputBuffer.position(videoBufferInfo.offset);
                        outputBuffer.limit(videoBufferInfo.offset + videoBufferInfo.size);

                        //
                        if (pts == 0) {
                            pts = videoBufferInfo.presentationTimeUs;
                        }
                        videoBufferInfo.presentationTimeUs = videoBufferInfo.presentationTimeUs - pts;

                        mediaMuxer.writeSampleData(videoTrackIndex, outputBuffer, videoBufferInfo);
                        if (encoder.get().onMediaInfoListener != null) {
                            encoder.get().onMediaInfoListener.onMediaTime((int)(videoBufferInfo.presentationTimeUs / 1000000));
                        }

                        videoEncoder.releaseOutputBuffer(outputBufferIndex, false);

                        outputBufferIndex = videoEncoder.dequeueOutputBuffer(videoBufferInfo, 0);
                    }
                }
            }
        }

        public void exit() {
            isExit = true;
        }
    }

    public interface OnMediaInfoListener {
        void onMediaTime(int times);
    }
}
