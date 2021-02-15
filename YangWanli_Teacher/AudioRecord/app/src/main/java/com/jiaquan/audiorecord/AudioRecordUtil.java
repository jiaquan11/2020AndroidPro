package com.jiaquan.audiorecord;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

public class AudioRecordUtil {
    private AudioRecord audioRecord;
    private int bufferSizeInBytes;
    private boolean start = false;
    private int readSize = 0;

    private OnRecordListener onRecordListener;

    public AudioRecordUtil() {
        bufferSizeInBytes = AudioRecord.getMinBufferSize(
                44100,
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT);

        this.audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC,
                44100,
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT,
                bufferSizeInBytes);
    }

    public void startRecord() {
        new Thread() {
            @Override
            public void run() {
                super.run();
                start = true;
                audioRecord.startRecording();

                byte[] audioData = new byte[bufferSizeInBytes];

                while (start) {
                    readSize = audioRecord.read(audioData, 0, bufferSizeInBytes);
                    if (onRecordListener != null) {
                        onRecordListener.recordByte(audioData, readSize);
                    }
                }

                if (audioRecord != null) {
                    audioRecord.stop();
                    audioRecord.release();
                    audioRecord = null;
                }
            }
        }.start();
    }

    public void stopRecord() {
        start = false;
    }

    public void setOnRecordListener(OnRecordListener onRecordListener) {
        this.onRecordListener = onRecordListener;
    }

    public interface OnRecordListener {
        void recordByte(byte[] audioData, int readSize);
    }
}
