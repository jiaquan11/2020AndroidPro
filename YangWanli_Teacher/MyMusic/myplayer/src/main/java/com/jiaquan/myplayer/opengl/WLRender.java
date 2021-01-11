package com.jiaquan.myplayer.opengl;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.view.Surface;

import com.jiaquan.myplayer.R;
import com.jiaquan.myplayer.log.MyLog;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class WLRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {
    private Context context;

    public static final int RENDER_YUV = 1;
    public static final int RENDER_MEDIACODEC = 2;

    private final float[] vertexData = {
//            -1f, 0f,
//            0f, -1f,
//            0f, 1f,
//
//            0f, 1f,
//            0f, -1f,
//            1f, 0f

            -1f, -1f,
            1f, -1f,
            -1f, 1f,
            1f, 1f
    };

    private final float[] textureData = {
            0f, 1f,
            1f, 1f,
            0f, 0f,
            1f, 0f
//
//            //纹理图像旋转操作
//            1f, 0f,
//            0f, 0f,
//            1f, 1f,
//            0f, 1f
    };

    private int renderType = RENDER_YUV;

    private FloatBuffer vertexBuffer;
    private FloatBuffer textureBuffer;

    //yuv
    private int program_yuv;
    private int avPosition_yuv;
    private int afPosition_yuv;

    private int sampler_y;
    private int sampler_u;
    private int sampler_v;

    private int[] textureId_yuv;
    private int width_yuv;
    private int height_yuv;
    private ByteBuffer y;
    private ByteBuffer u;
    private ByteBuffer v;

    //mediacodec
    private int program_mediacodec;
    private int avPosition_mediacodec;
    private int afPosition_mediacodec;
    private int samplerOES_mediacodec;
    private int textureId_mediacodec;
    private SurfaceTexture surfaceTexture;
    private Surface surface;

    public void setRenderType(int renderType) {
        this.renderType = renderType;
    }

    public interface OnSurfaceCreateListener {
        void onSurfaceCreate(Surface surface);
    }

    private OnSurfaceCreateListener onSurfaceCreateListener;

    public void setOnSurfaceCreateListener(OnSurfaceCreateListener onSurfaceCreateListener) {
        this.onSurfaceCreateListener = onSurfaceCreateListener;
    }

    private OnRenderListener onRenderListener;

    public interface OnRenderListener {
        void onRender();
    }

    public void setOnRenderListener(OnRenderListener onRenderListener) {
        this.onRenderListener = onRenderListener;
    }

    public WLRender(Context ctx) {
        MyLog.i("construct WLRender in");
        context = ctx;

        vertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);

        vertexBuffer.position(0);

        textureBuffer = ByteBuffer.allocateDirect(textureData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(textureData);

        textureBuffer.position(0);

        MyLog.i("construct WLRender end");
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        MyLog.i("WLRender onSurfaceCreated in");
        initRenderYUV();

        initRenderMediaCodec();
        MyLog.i("WLRender onSurfaceCreated end");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
    }

    /*
    * GLSurfaceView自己内部封装了渲染操作，当在onDrawFrame中 opengl绘制完成之后，就自动渲染画面出来了
    * */
    @Override
    public void onDrawFrame(GL10 gl) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//0 0 0是黑色清屏 1 1 1是白色

        if (renderType == RENDER_YUV) {
            renderYUV();
        } else if (renderType == RENDER_MEDIACODEC) {
            renderMediaCodec();
        }

        //将绘制操作放在外面，每次都会进行绘制一次，暂时解决概率性的闪屏问题
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    /*
    * 当硬解出画面数据时，会存放在surface这个屏幕缓冲区中，他然后就会给到
    * SurfaceTexture中，SurfaceTexture会马上回调onFrameAvailable，表示有有效实际
    * 数据画面到达，那么接着就可以进行渲染操作，调用一下requestRender请求onDrawFrame,
    * 在onDrawFrame中surfaceTexture.updateTexImage();表示将缓存数据刷到前台更新，同时
    * 使用opengl的操作绘制出来，最后GLSurfaceView自己内部封装的渲染操作渲染出来
    * */
    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (onRenderListener != null) {
            MyLog.i("onFrameAvailable in");
            onRenderListener.onRender();
            MyLog.i("onFrameAvailable out");
        }
    }

    private void initRenderYUV() {
        String vertexSource = WLShaderUtil.readRawTxt(context, R.raw.vertex_shader);
        String fragmentSource = WLShaderUtil.readRawTxt(context, R.raw.fragment_yuv);
        program_yuv = WLShaderUtil.createProgram(vertexSource, fragmentSource);
        avPosition_yuv = GLES20.glGetAttribLocation(program_yuv, "av_Position");
        afPosition_yuv = GLES20.glGetAttribLocation(program_yuv, "af_Position");

        sampler_y = GLES20.glGetUniformLocation(program_yuv, "sampler_y");
        sampler_u = GLES20.glGetUniformLocation(program_yuv, "sampler_u");
        sampler_v = GLES20.glGetUniformLocation(program_yuv, "sampler_v");

        textureId_yuv = new int[3];
        GLES20.glGenTextures(3, textureId_yuv, 0);

        for (int i = 0; i < 3; i++) {
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId_yuv[i]);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        }

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
    }

    public void setYUVRenderData(int width, int height, byte[] y, byte[] u, byte[] v) {
        this.width_yuv = width;
        this.height_yuv = height;
        this.y = ByteBuffer.wrap(y);
        this.u = ByteBuffer.wrap(u);
        this.v = ByteBuffer.wrap(v);
    }

    private void renderYUV() {
        if ((width_yuv > 0) && (height_yuv > 0) && (y != null) && (u != null) && (v != null)) {
            GLES20.glUseProgram(program_yuv);

            GLES20.glEnableVertexAttribArray(avPosition_yuv);
            GLES20.glVertexAttribPointer(avPosition_yuv, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);

            GLES20.glEnableVertexAttribArray(afPosition_yuv);
            GLES20.glVertexAttribPointer(afPosition_yuv, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId_yuv[0]);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv, height_yuv, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, y);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId_yuv[1]);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, u);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId_yuv[2]);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, v);

            GLES20.glUniform1i(sampler_y, 0);
            GLES20.glUniform1i(sampler_u, 1);
            GLES20.glUniform1i(sampler_v, 2);

            y.clear();
            u.clear();
            v.clear();
            y = null;
            u = null;
            v = null;

//            GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
//
//            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        }
    }

    private void initRenderMediaCodec() {
        MyLog.i("initRenderMediaCodec in");
        String vertexSource = WLShaderUtil.readRawTxt(context, R.raw.vertex_shader);
        String fragmentSource = WLShaderUtil.readRawTxt(context, R.raw.fragment_mediacodec);
        program_mediacodec = WLShaderUtil.createProgram(vertexSource, fragmentSource);
        avPosition_mediacodec = GLES20.glGetAttribLocation(program_mediacodec, "av_Position");
        afPosition_mediacodec = GLES20.glGetAttribLocation(program_mediacodec, "af_Position");

        samplerOES_mediacodec = GLES20.glGetUniformLocation(program_mediacodec, "sTexture");

        int[] textrueids = new int[1];
        GLES20.glGenTextures(1, textrueids, 0);
        textureId_mediacodec = textrueids[0];

        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureId_mediacodec);

        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);

        surfaceTexture = new SurfaceTexture(textureId_mediacodec);
        surface = new Surface(surfaceTexture);
        surfaceTexture.setOnFrameAvailableListener(this);

        if (onSurfaceCreateListener != null) {
            onSurfaceCreateListener.onSurfaceCreate(surface);
        }

        MyLog.i("initRenderMediaCodec out");
    }

    private void renderMediaCodec() {
        MyLog.i("renderMediaCodec in");
        surfaceTexture.updateTexImage();

        GLES20.glUseProgram(program_mediacodec);

        GLES20.glEnableVertexAttribArray(avPosition_mediacodec);
        GLES20.glVertexAttribPointer(avPosition_mediacodec, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);

        GLES20.glEnableVertexAttribArray(afPosition_mediacodec);
        GLES20.glVertexAttribPointer(afPosition_mediacodec, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textureId_mediacodec);

        GLES20.glUniform1i(samplerOES_mediacodec, 0);

//        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);
        MyLog.i("renderMediaCodec out");
    }
}
