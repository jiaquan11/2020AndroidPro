package com.jiaquan.myplayer.player;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.view.Surface;

import com.jiaquan.myplayer.TimeInfoBean;
import com.jiaquan.myplayer.listener.OnCompleteListener;
import com.jiaquan.myplayer.listener.OnErrorListener;
import com.jiaquan.myplayer.listener.OnLoadListener;
import com.jiaquan.myplayer.listener.OnPauseResumeListener;
import com.jiaquan.myplayer.listener.OnPcmInfoListener;
import com.jiaquan.myplayer.listener.OnPreparedListener;
import com.jiaquan.myplayer.listener.OnRecordTimeListener;
import com.jiaquan.myplayer.listener.OnTimeInfoListener;
import com.jiaquan.myplayer.listener.OnVolumeDBListener;
import com.jiaquan.myplayer.log.MyLog;
import com.jiaquan.myplayer.muteenum.MuteEnum;
import com.jiaquan.myplayer.opengl.WLGLSurfaceView;
import com.jiaquan.myplayer.opengl.WLRender;
import com.jiaquan.myplayer.util.WLVideoSupportUtil;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

public class WLPlayer {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec");
        System.loadLibrary("avdevice");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("postproc");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
    }

    private static String source = null;
    private static boolean playNext = false;
    private static int duration = -1;
    private static int volumePercent = 100;
    private static MuteEnum muteEnum = MuteEnum.MUTE_CENTER;
    private static float speed = 1.0f;
    private static float pitch = 1.0f;
    private static boolean isInitMediaCodec = false;

    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private Surface surface;
    private MediaCodec.BufferInfo info;

    private WLGLSurfaceView wlglSurfaceView = null;

    public void setWlglSurfaceView(WLGLSurfaceView wlglSurfaceView) {
        this.wlglSurfaceView = wlglSurfaceView;
        wlglSurfaceView.getWlRender().setOnSurfaceCreateListener(new WLRender.OnSurfaceCreateListener() {
            @Override
            public void onSurfaceCreate(Surface s) {
                if (surface == null) {
                    surface = s;
                    MyLog.i("onSurfaceCreate");
                }
            }
        });
    }

    private OnPreparedListener onPreparedListener = null;

    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    private OnLoadListener onLoadListener = null;

    public void setOnLoadListener(OnLoadListener onLoadListener) {
        this.onLoadListener = onLoadListener;
    }

    private OnPauseResumeListener onPauseResumeListener = null;

    public void setOnPauseResumeListener(OnPauseResumeListener onPauseResumeListener) {
        this.onPauseResumeListener = onPauseResumeListener;
    }

    private OnTimeInfoListener onTimeInfoListener = null;

    public void setOnTimeInfoListener(OnTimeInfoListener onTimeInfoListener) {
        this.onTimeInfoListener = onTimeInfoListener;
    }

    private OnErrorListener onErrorListener = null;

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    private OnCompleteListener onCompleteListener = null;

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
    }

    private OnVolumeDBListener onVolumeDBListener = null;

    public void setOnVolumeDBListener(OnVolumeDBListener onVolumeDBListener) {
        this.onVolumeDBListener = onVolumeDBListener;
    }

    private OnRecordTimeListener onRecordTimeListener = null;

    public void setOnRecordTimeListener(OnRecordTimeListener onRecordTimeListener) {
        this.onRecordTimeListener = onRecordTimeListener;
    }

    private OnPcmInfoListener onPcmInfoListener = null;

    public void setOnPcmInfoListener(OnPcmInfoListener onPcmInfoListener) {
        this.onPcmInfoListener = onPcmInfoListener;
    }

    private static TimeInfoBean timeInfoBean = null;

    public void setSource(String source) {
        this.source = source;
    }

    public WLPlayer() {

    }

    public void prepared() {
        if (TextUtils.isEmpty(source)) {
            MyLog.i("source must not be empty");
            return;
        }

//        onCallLoad(true);

        new Thread(new Runnable() {
            @Override
            public void run() {
                _prepared(source);
            }
        }).start();
    }

    public void start() {
        if (TextUtils.isEmpty(source)) {
            MyLog.i("source must not be empty");
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                setVolume(volumePercent);
                setMute(muteEnum);
                setPitch(pitch);
                setSpeed(speed);
                _start();
            }
        }).start();
    }

    public void onCallPrepared() {
        if (onPreparedListener != null) {
            onPreparedListener.onPrepared();
        }
    }

    public void onCallLoad(boolean load) {
        if (onLoadListener != null) {
            onLoadListener.onLoad(load);
        }
    }

    public void onCallTimeInfo(int currentTime, int totalTime) {
        if (onTimeInfoListener != null) {
            if (timeInfoBean == null) {
                timeInfoBean = new TimeInfoBean();
            }
            timeInfoBean.setCurrentTime(currentTime);
            timeInfoBean.setTotalTime(totalTime);
            onTimeInfoListener.onTimeInfo(timeInfoBean);
        }
    }

    public void onCallError(int code, String msg) {
        stop();

        if (onErrorListener != null) {
            onErrorListener.onError(code, msg);
        }
    }

    public void onCallComplete() {
        stop();

        if (onCompleteListener != null) {
            onCompleteListener.onComplete();
        }
    }

    public void onCallNext() {
        if (playNext) {
            playNext = false;
            prepared();
        }
    }

    public void onCallVolumeDB(int db) {
        if (onVolumeDBListener != null) {
            onVolumeDBListener.onDBValue(db);
        }
    }

    public void onCallPcmInfo(byte[] buffer, int bufferSize) {
        if (onPcmInfoListener != null) {
            onPcmInfoListener.onPcmInfo(buffer, bufferSize);
        }
    }

    public void onCallPcmRate(int samplerate, int bit, int channels) {
        if (onPcmInfoListener != null) {
            onPcmInfoListener.onPcmRate(samplerate, bit, channels);
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v) {
        MyLog.i("获取到YUV数据渲染");
        if (wlglSurfaceView != null) {
            wlglSurfaceView.getWlRender().setRenderType(WLRender.RENDER_YUV);
            wlglSurfaceView.setYUVData(width, height, y, u, v);
        }
    }

    public boolean onCallIsSupportMediaCodec(String ffcodecname) {
        return WLVideoSupportUtil.isSupportCodec(ffcodecname);
    }

    //video

    /**
     * 初始化视频解码器
     *
     * @param codecName
     * @param width
     * @param height
     * @param csd_0
     * @param csd_1
     */
    public void onCallinitMediaCodec(String codecName, int width, int height, byte[] csd_0, byte[] csd_1) {
        if (surface != null) {
            try {
                wlglSurfaceView.getWlRender().setRenderType(WLRender.RENDER_MEDIACODEC);

                String mime = WLVideoSupportUtil.findVideoCodecName(codecName);
                mediaFormat = MediaFormat.createVideoFormat(mime, width, height);
                mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, width * height);
                mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd_0));
                mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd_1));
                MyLog.i(mediaFormat.toString());
                mediaCodec = MediaCodec.createDecoderByType(mime);

                info = new MediaCodec.BufferInfo();
                mediaCodec.configure(mediaFormat, surface, null, 0);
                mediaCodec.start();
            } catch (Exception e) {
                e.printStackTrace();
            }

            MyLog.i("onCallinitMediaCodec end");
        } else {
            if (onErrorListener != null) {
                onErrorListener.onError(2001, "surface is null");
            }
        }
    }

    public void onCallDecodeVPacket(int datasize, byte[] data) {
        MyLog.i("onCallDecodeVPacket in");
        if ((surface != null) && (datasize > 0) && (data != null) && (mediaCodec != null)) {
            try {
                int inputBufferIndex = mediaCodec.dequeueInputBuffer(10);
                if (inputBufferIndex >= 0) {
                    ByteBuffer byteBuffer = mediaCodec.getInputBuffers()[inputBufferIndex];
                    byteBuffer.clear();
                    byteBuffer.put(data);
                    mediaCodec.queueInputBuffer(inputBufferIndex, 0, datasize, 0, 0);
                }
                int outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
                while (outputBufferIndex >= 0) {
                    mediaCodec.releaseOutputBuffer(outputBufferIndex, true);
                    outputBufferIndex = mediaCodec.dequeueOutputBuffer(info, 10);
                    MyLog.i("mediaCodec releaseOutputBuffer");
                }
                MyLog.i("onCallDecodeVPacket out");
            }catch (Exception e){
                e.printStackTrace();
            }
        }
    }

    private void releaseVMediaCodec() {
        if (mediaCodec != null) {
            try{
                mediaCodec.flush();
                mediaCodec.stop();
                mediaCodec.release();
            }catch (Exception e){
                e.printStackTrace();
            }

            mediaCodec = null;
            mediaFormat = null;
            info = null;
        }
    }

    public void pause() {
        _pause();
        if (onPauseResumeListener != null) {
            onPauseResumeListener.onPause(true);
        }
    }

    public void resume() {
        _resume();
        if (onPauseResumeListener != null) {
            onPauseResumeListener.onPause(false);
        }
    }

    public void seek(int secds) {
        _seek(secds);
    }

    public void stop() {
        timeInfoBean = null;
        duration = -1;

        stopRecord();

        new Thread(new Runnable() {
            @Override
            public void run() {
                _stop();

                releaseVMediaCodec();
            }
        }).start();
    }

    public void playNext(String url) {
        source = url;
        playNext = true;
        stop();
    }

    public int getDuration() {
        if (duration < 0) {
            duration = _duration();
        }
        return duration;
    }

    public void setVolume(int percent) {
        if ((percent >= 0) && (percent <= 100)) {
            volumePercent = percent;
            _volume(percent);
        }
    }

    public int getVolumePercent() {
        return volumePercent;
    }

    public void setMute(MuteEnum mute) {
        muteEnum = mute;
        _mute(mute.getValue());
    }

    public void setPitch(float p) {
        pitch = p;
        _pitch(pitch);
    }

    public void setSpeed(float s) {
        speed = s;
        _speed(speed);
    }

    public void startRecord(File outfile) {
        if (!isInitMediaCodec) {
            audioSamplerate = _samplerate();

            if (audioSamplerate > 0) {
                isInitMediaCodec = true;
                initMediaCodec(audioSamplerate, outfile);
                _startstopRecord(true);
                MyLog.i("开始录制....");
            }
        }
    }

    public void stopRecord() {
        if (isInitMediaCodec) {
            _startstopRecord(false);
            releaseMediacodec();
            MyLog.i("完成录制....");
        }
    }

    public void pauseRecord() {
        _startstopRecord(false);
        MyLog.i("暂停录制....");
    }

    public void resumeRecord() {
        _startstopRecord(true);
        MyLog.i("恢复录制....");
    }

    public void cutAudioPlay(int start_time, int end_time, boolean showPcm) {
        if (_cutAudioPlay(start_time, end_time, showPcm)) {
            start();
        } else {
            stop();
            onCallError(2001, "cutAudioPlay params is wrong!");
        }
    }

    private native void _prepared(String source);

    private native void _start();

    private native void _pause();

    private native void _resume();

    private native void _stop();

    private native void _seek(int secds);

    private native int _duration();

    private native void _volume(int percent);

    private native void _mute(int mute);

    private native void _pitch(float pitch);

    private native void _speed(float speed);

    private native int _samplerate();

    private native void _startstopRecord(boolean start);

    private native boolean _cutAudioPlay(int start_time, int end_time, boolean showPcm);

    //mediacodec
    private MediaFormat encoderFormat = null;
    private MediaCodec encoder = null;
    private FileOutputStream fileOutputStream = null;
    private MediaCodec.BufferInfo bufferInfo = null;
    private int perpcmSize = 0;
    private byte[] outByteBuffer = null;
    private int aacSampleRateType = 4;
    private double recordTime = 0;
    private int audioSamplerate = 0;

    private void initMediaCodec(int samplerate, File outfile) {
        try {
            aacSampleRateType = getADTSSampleRate(samplerate);

            encoderFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, samplerate, 2);
            encoderFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);//码率
            encoderFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);//AAC profile
            encoderFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 4096);//输入编码的最大pcm数据大小
            encoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);//创建音频编码器
            bufferInfo = new MediaCodec.BufferInfo();

            if (encoder == null) {
                MyLog.e("create encoder wrong");
                return;
            }

            recordTime = 0;

            encoder.configure(encoderFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);//配置编码器
            fileOutputStream = new FileOutputStream(outfile);
            encoder.start();//启动音频编码器
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void encodePcmToAAC(byte[] buffer, int size) {
        MyLog.i("encodePcmToAAC buffer size: " + size);
        if ((buffer != null) && (encoder != null)) {
            recordTime += size * 1.0 / (audioSamplerate * 2 * 2);
//            MyLog.i("recordTime: " + recordTime);
            if (onRecordTimeListener != null) {
                onRecordTimeListener.onRecordTime((int) recordTime);
            }

            int inputBufferIndex = encoder.dequeueInputBuffer(0);//获取到编码输入buffer的可用索引
            if (inputBufferIndex >= 0) {
                ByteBuffer byteBuffer = encoder.getInputBuffers()[inputBufferIndex];//根据索引获取编码输入可用的空闲buffer
                byteBuffer.clear();
                byteBuffer.put(buffer);//将pcm数据放入空用buffer中
                encoder.queueInputBuffer(inputBufferIndex, 0, size, 0, 0);//编码器入队进行编码
            }

            int index = encoder.dequeueOutputBuffer(bufferInfo, 0);//获取编码器码流输出buffer的索引
            while (index >= 0) {
                try {
                    perpcmSize = bufferInfo.size + 7;
                    outByteBuffer = new byte[perpcmSize];

                    ByteBuffer byteBuffer = encoder.getOutputBuffers()[index];//获取到编码器输出的码流buffer
                    byteBuffer.position(bufferInfo.offset);
                    byteBuffer.limit(bufferInfo.offset + bufferInfo.size);

                    addADTSHeader(outByteBuffer, perpcmSize, aacSampleRateType);//增加AAC码流头

                    byteBuffer.get(outByteBuffer, 7, bufferInfo.size);//将编码码流数据放入AAC码流头后面存放
                    byteBuffer.position(bufferInfo.offset);

                    fileOutputStream.write(outByteBuffer, 0, perpcmSize);//将完整的一帧音频码流数据写入文件

                    encoder.releaseOutputBuffer(index, false);//取出码流数据后，释放这个buffer,返回给队列中循环使用

                    index = encoder.dequeueOutputBuffer(bufferInfo, 0);
                    outByteBuffer = null;

//                    MyLog.i("编码....");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void addADTSHeader(byte[] packet, int packetLen, int samplerate) {
        int profile = 2; // AAC LC
        int freqIdx = samplerate; // samplerate
        int chanCfg = 2; // CPE

        // fill in ADTS data
        packet[0] = (byte) 0xFF;
        packet[1] = (byte) 0xF9;
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    private int getADTSSampleRate(int samplerate) {
        int rate = 4;
        switch (samplerate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }

    private void releaseMediacodec() {
        if (encoder == null) {
            return;
        }

        try {
            recordTime = 0;

            fileOutputStream.close();
            fileOutputStream = null;

            encoder.stop();
            encoder.release();
            encoder = null;
            encoderFormat = null;
            bufferInfo = null;

            isInitMediaCodec = false;

            MyLog.i("录制完成....");
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fileOutputStream != null) {
                try {
                    fileOutputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                fileOutputStream = null;
            }
        }
    }
}
