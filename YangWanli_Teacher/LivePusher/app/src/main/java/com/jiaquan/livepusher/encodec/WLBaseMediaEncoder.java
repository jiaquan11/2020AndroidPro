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

    private MediaCodec audioEncoder;
    private MediaFormat audioFormat;
    private MediaCodec.BufferInfo audioBufferInfo;
    private long audioPts = 0;
    private int sampleRate;

    private MediaMuxer mediaMuxer;
    private boolean encoderStart;
    private boolean audioExit;
    private boolean videoExit;

    private WLEGLMediaThread wleglMediaThread;
    private VideoEncoderThread videoEncoderThread;
    private AudioEncoderThread audioEncoderThread;

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

    public void initEncoder(EGLContext eglContext, String savePath, int width, int height, int sampleRate, int channelCount) {
        this.width = width;
        this.height = height;
        this.eglContext = eglContext;

        initMediaEncoder(savePath, width, height, sampleRate, channelCount);
    }

    public void startRecord() {
        if ((surface != null) && (eglContext != null)) {
            audioExit = false;
            videoExit = false;
            encoderStart = false;
            audioPts = 0;

            wleglMediaThread = new WLEGLMediaThread(new WeakReference<WLBaseMediaEncoder>(this));
            videoEncoderThread = new VideoEncoderThread(new WeakReference<WLBaseMediaEncoder>(this));
            audioEncoderThread = new AudioEncoderThread(new WeakReference<WLBaseMediaEncoder>(this));
            wleglMediaThread.isCreate = true;
            wleglMediaThread.isChange = true;

            wleglMediaThread.start();
            videoEncoderThread.start();
            audioEncoderThread.start();
        }
    }

    public void stopRecord() {
        if ((wleglMediaThread != null) && (videoEncoderThread != null) && (audioEncoderThread != null)) {
            videoEncoderThread.exit();
            audioEncoderThread.exit();
            wleglMediaThread.onDestroy();
            videoEncoderThread = null;
            audioEncoderThread = null;
            wleglMediaThread = null;
        }
    }

    private void initMediaEncoder(String savePath, int width, int height, int sampleRate, int channelCount) {
        try {
            mediaMuxer = new MediaMuxer(savePath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);

            initVideoEncoder(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
            initAudioEncoder(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, channelCount);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void initAudioEncoder(String mimeType, int sampleRate, int channelCount) {
        try {
            this.sampleRate = sampleRate;

            audioBufferInfo = new MediaCodec.BufferInfo();
            audioFormat = MediaFormat.createAudioFormat(mimeType, sampleRate, channelCount);
            audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
            audioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096);

            audioEncoder = MediaCodec.createEncoderByType(mimeType);
            audioEncoder.configure(audioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        } catch (IOException e) {
            e.printStackTrace();
            audioBufferInfo = null;
            audioFormat = null;
            audioEncoder = null;
        }
    }

    //初始化硬件编码器
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

            //编码配置后使用createInputSurface()为输入数据创建目标Surface
            surface = videoEncoder.createInputSurface();
        } catch (IOException e) {
            e.printStackTrace();
            videoEncoder = null;
            videoFormat = null;
            videoBufferInfo = null;
        }
    }

    public void putPCMData(byte[] buffer, int size) {
        if ((audioEncoderThread != null) && !audioEncoderThread.isExit && (buffer != null) && (size > 0)) {
            int inputBufferIndex = audioEncoder.dequeueInputBuffer(0);
            if (inputBufferIndex >= 0) {
                ByteBuffer byteBuffer = audioEncoder.getInputBuffers()[inputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(buffer);
                long pts = getAudioPts(size, sampleRate);
                audioEncoder.queueInputBuffer(inputBufferIndex, 0, size, pts, 0);
            }
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
            //传入的是硬编编码器创建的surface,用于存放编码数据，eglContext是摄像头预览的egl上下文，将预览数据进行编码
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

        private int videoTrackIndex = -1;

        VideoEncoderThread(WeakReference<WLBaseMediaEncoder> encoder) {
            this.encoder = encoder;
            videoEncoder = encoder.get().videoEncoder;
            videoBufferInfo = encoder.get().videoBufferInfo;
            mediaMuxer = encoder.get().mediaMuxer;

            videoTrackIndex = -1;
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

                    encoder.get().videoExit = true;
                    if (encoder.get().audioExit) {
                        mediaMuxer.stop();
                        mediaMuxer.release();
                        mediaMuxer = null;
                        Log.i("WLBaseMediaEncoder", "Video 录制完成!!!");
                    }
                    break;
                }

                int outputBufferIndex = videoEncoder.dequeueOutputBuffer(videoBufferInfo, 0);
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    videoTrackIndex = mediaMuxer.addTrack(videoEncoder.getOutputFormat());
                    if (encoder.get().audioEncoderThread.audioTrackIndex != -1) {
                        mediaMuxer.start();

                        encoder.get().encoderStart = true;
                    }
                } else {
                    while (outputBufferIndex >= 0) {
                        if (encoder.get().encoderStart) {
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
                                encoder.get().onMediaInfoListener.onMediaTime((int) (videoBufferInfo.presentationTimeUs / 1000000));
                            }
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

    static class AudioEncoderThread extends Thread {
        private WeakReference<WLBaseMediaEncoder> encoder;
        private boolean isExit;

        private MediaCodec audioEncoder;
        private MediaCodec.BufferInfo bufferInfo;
        private MediaMuxer mediaMuxer;

        private int audioTrackIndex = -1;
        long pts;

        AudioEncoderThread(WeakReference<WLBaseMediaEncoder> encoder) {
            this.encoder = encoder;

            audioEncoder = encoder.get().audioEncoder;
            bufferInfo = encoder.get().audioBufferInfo;
            mediaMuxer = encoder.get().mediaMuxer;

            audioTrackIndex = -1;
        }

        @Override
        public void run() {
            super.run();

            pts = 0;
            audioTrackIndex = -1;
            isExit = false;

            audioEncoder.start();

            while (true) {
                if (isExit) {
                    audioEncoder.stop();
                    audioEncoder.release();
                    audioEncoder = null;

                    encoder.get().audioExit = true;
                    if (encoder.get().videoExit) {
                        mediaMuxer.stop();
                        mediaMuxer.release();
                        mediaMuxer = null;

                        Log.i("WLBaseMediaEncoder", "Audio 录制完成!!!");
                    }
                    break;
                }

                int outputBufferIndex = audioEncoder.dequeueOutputBuffer(bufferInfo, 0);
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    if (mediaMuxer != null) {
                        audioTrackIndex = mediaMuxer.addTrack(audioEncoder.getOutputFormat());
                        if (encoder.get().videoEncoderThread.videoTrackIndex != -1) {
                            mediaMuxer.start();

                            encoder.get().encoderStart = true;
                        }
                    }
                } else {
                    while (outputBufferIndex >= 0) {
                        if (encoder.get().encoderStart) {
                            ByteBuffer outputBuffer = audioEncoder.getOutputBuffers()[outputBufferIndex];
                            outputBuffer.position(bufferInfo.offset);
                            outputBuffer.limit(bufferInfo.offset + bufferInfo.size);

                            if (pts == 0) {
                                pts = bufferInfo.presentationTimeUs;
                            }
                            bufferInfo.presentationTimeUs = bufferInfo.presentationTimeUs - pts;

                            mediaMuxer.writeSampleData(audioTrackIndex, outputBuffer, bufferInfo);
                        }

                        audioEncoder.releaseOutputBuffer(outputBufferIndex, false);
                        outputBufferIndex = audioEncoder.dequeueOutputBuffer(bufferInfo, 0);
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

    private long getAudioPts(int size, int sampleRate) {
        audioPts += (long) (1.0 * size / (sampleRate * 2 * 2) * 1000000.0);
        return audioPts;
    }
}
