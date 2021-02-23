package com.jiaquan.livepusher.push;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;

import com.jiaquan.livepusher.egl.EglHelper;
import com.jiaquan.livepusher.egl.WLEGLSurfaceView;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLContext;

public abstract class WLBasePushEncoder {
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

    private WLEGLMediaThread wleglMediaThread;
    private VideoEncoderThread videoEncoderThread;
    private AudioEncoderThread audioEncoderThread;
    private AudioRecordUtil audioRecordUtil;

    private WLEGLSurfaceView.WLGLRender wlglRender = null;

    private int mRenderMode = RENDERMODE_CONTINUOUSLY;

    public final static int RENDERMODE_WHEN_DIRTY = 0;
    public final static int RENDERMODE_CONTINUOUSLY = 1;

    private OnMediaInfoListener onMediaInfoListener;

    public interface OnMediaInfoListener {
        void onMediaTime(int times);

        void onSPSPPSInfo(byte[] sps, byte[] pps);

        void onVideoInfo(byte[] data, boolean keyframe);

        void onAudioInfo(byte[] data);
    }

    public void setOnMediaInfoListener(OnMediaInfoListener onMediaInfoListener) {
        this.onMediaInfoListener = onMediaInfoListener;
    }

    public WLBasePushEncoder(Context context) {

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

    public void initEncoder(EGLContext eglContext, int width, int height) {
        this.width = width;
        this.height = height;
        this.eglContext = eglContext;

        initMediaEncoder(width, height, 44100, 2);
    }

    public void startRecord() {
        if ((surface != null) && (eglContext != null)) {
            audioPts = 0;

            wleglMediaThread = new WLEGLMediaThread(new WeakReference<WLBasePushEncoder>(this));
            videoEncoderThread = new VideoEncoderThread(new WeakReference<WLBasePushEncoder>(this));
            audioEncoderThread = new AudioEncoderThread(new WeakReference<WLBasePushEncoder>(this));
            wleglMediaThread.isCreate = true;
            wleglMediaThread.isChange = true;

            wleglMediaThread.start();
            videoEncoderThread.start();
            audioEncoderThread.start();

            audioRecordUtil.startRecord();
        }
    }

    public void stopRecord() {
        Log.i("WLBasePushEncoder", "stopRecord in");
        if ((wleglMediaThread != null) && (videoEncoderThread != null) && (audioEncoderThread != null)) {
            audioRecordUtil.stopRecord();

            videoEncoderThread.exit();
            audioEncoderThread.exit();
            wleglMediaThread.onDestroy();
            videoEncoderThread = null;
            audioEncoderThread = null;
            wleglMediaThread = null;
        }
        Log.i("WLBasePushEncoder", "stopRecord out");
    }

    private void initMediaEncoder(int width, int height, int sampleRate, int channelCount) {
        initVideoEncoder(MediaFormat.MIMETYPE_VIDEO_AVC, width, height);
        initAudioEncoder(MediaFormat.MIMETYPE_AUDIO_AAC, sampleRate, channelCount);

        initPCMRecord();
    }

    private void initPCMRecord(){
        audioRecordUtil = new AudioRecordUtil();
        audioRecordUtil.setOnRecordListener(new AudioRecordUtil.OnRecordListener() {
            @Override
            public void recordByte(byte[] audioData, int readSize) {
                if (audioRecordUtil.isStart()){
                    putPCMData(audioData, readSize);
                }
            }
        });
    }

    private void initAudioEncoder(String mimeType, int sampleRate, int channelCount) {
        try {
            this.sampleRate = sampleRate;

            audioBufferInfo = new MediaCodec.BufferInfo();
            audioFormat = MediaFormat.createAudioFormat(mimeType, sampleRate, channelCount);
            audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);
            audioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096 * 10);

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
        private WeakReference<WLBasePushEncoder> encoder;
        private EglHelper eglHelper;
        private Object object;

        private boolean isExit = false;
        private boolean isCreate = false;
        private boolean isChange = false;
        private boolean isStart = false;

        public WLEGLMediaThread(WeakReference<WLBasePushEncoder> encoder) {
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
            if (encoder.get().wlglRender != null && eglHelper != null) {
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
        private WeakReference<WLBasePushEncoder> encoder;

        private boolean isExit;

        private MediaCodec videoEncoder;
        private MediaCodec.BufferInfo videoBufferInfo;

        private long pts;
        private byte[] sps;
        private byte[] pps;
        private boolean keyFrame = false;

        public VideoEncoderThread(WeakReference<WLBasePushEncoder> encoder) {
            this.encoder = encoder;
            videoEncoder = encoder.get().videoEncoder;
            videoBufferInfo = encoder.get().videoBufferInfo;
        }

        @Override
        public void run() {
            super.run();
            pts = 0;
            isExit = false;
            videoEncoder.start();
            while (true) {
//                Log.i("WLBasePushEncoder", "VideoEncoderThread isExit: " + isExit);
                if (isExit) {
                    videoEncoder.stop();
                    videoEncoder.release();
                    videoEncoder = null;
                    Log.d("WLBasePushEncoder", "VideoEncodecThread video 录制完成");
                    break;
                }

//                keyFrame = false;

                int outputBufferIndex = videoEncoder.dequeueOutputBuffer(videoBufferInfo, 0);
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    Log.i("WLBasePushEncoder", "Video INFO_OUTPUT_FORMAT_CHANGED");

                    ByteBuffer spsb = videoEncoder.getOutputFormat().getByteBuffer("csd-0");
                    sps = new byte[spsb.remaining()];
                    spsb.get(sps, 0, sps.length);

                    ByteBuffer ppsb = videoEncoder.getOutputFormat().getByteBuffer("csd-1");
                    pps = new byte[ppsb.remaining()];
                    ppsb.get(pps, 0, pps.length);

                    Log.i("WLBasePushEncoder", "sps:" + byteToHex(sps));
                    Log.i("WLBasePushEncoder", "pps:" + byteToHex(pps));
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

                        byte[] data = new byte[outputBuffer.remaining()];
                        outputBuffer.get(data, 0, data.length);
//                        Log.i("WLBasePushEncoder", "data:" + byteToHex(data));

                        keyFrame = false;
                        if (videoBufferInfo.flags == MediaCodec.BUFFER_FLAG_KEY_FRAME) {
                            keyFrame = true;
                            if (encoder.get().onMediaInfoListener != null) {//只要遇到关键帧，在发关键帧数据包之前都要发一次SPS和PPS
                                encoder.get().onMediaInfoListener.onSPSPPSInfo(sps, pps);
                            }
                        }
                        if (encoder.get().onMediaInfoListener != null) {
                            encoder.get().onMediaInfoListener.onVideoInfo(data, keyFrame);
                            encoder.get().onMediaInfoListener.onMediaTime((int) (videoBufferInfo.presentationTimeUs / 1000000));
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
        private WeakReference<WLBasePushEncoder> encoder;
        private boolean isExit;

        private MediaCodec audioEncoder;
        private MediaCodec.BufferInfo bufferInfo;

        long pts;

        public AudioEncoderThread(WeakReference<WLBasePushEncoder> encoder) {
            this.encoder = encoder;

            audioEncoder = encoder.get().audioEncoder;
            bufferInfo = encoder.get().audioBufferInfo;
        }

        @Override
        public void run() {
            super.run();

            pts = 0;
            isExit = false;

            audioEncoder.start();
            while (true) {
                if (isExit) {
                    audioEncoder.stop();
                    audioEncoder.release();
                    audioEncoder = null;
                    Log.i("WLBasePushEncoder", "Audio 录制完成!");
                    break;
                }

                int outputBufferIndex = audioEncoder.dequeueOutputBuffer(bufferInfo, 0);
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {

                } else {
                    while (outputBufferIndex >= 0) {
                        ByteBuffer outputBuffer = audioEncoder.getOutputBuffers()[outputBufferIndex];
                        outputBuffer.position(bufferInfo.offset);
                        outputBuffer.limit(bufferInfo.offset + bufferInfo.size);

                        if (pts == 0) {
                            pts = bufferInfo.presentationTimeUs;
                        }
                        bufferInfo.presentationTimeUs = bufferInfo.presentationTimeUs - pts;

                        byte[] data = new byte[outputBuffer.remaining()];
                        outputBuffer.get(data, 0, data.length);
//                        Log.i("WLBasePushEncoder", "audio data:" + byteToHex(data));
                        if (encoder.get().onMediaInfoListener != null){
                            encoder.get().onMediaInfoListener.onAudioInfo(data);
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

    private long getAudioPts(int size, int sampleRate) {
        audioPts += (long) (1.0 * size / (sampleRate * 2 * 2) * 1000000.0);
        return audioPts;
    }

    //将字节数组转化十六进制字符串
    public static String byteToHex(byte[] bytes) {
        StringBuffer stringBuffer = new StringBuffer();
        for (int i = 0; i < bytes.length; i++) {
            String hex = Integer.toHexString(bytes[i]);
            if (hex.length() == 1) {
                stringBuffer.append("0" + hex);
            } else {
                stringBuffer.append(hex);
            }
            if (i > 20) {
                break;
            }
        }
        return stringBuffer.toString();
    }
}
