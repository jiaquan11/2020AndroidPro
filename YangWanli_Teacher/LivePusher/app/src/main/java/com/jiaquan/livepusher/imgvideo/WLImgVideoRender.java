package com.jiaquan.livepusher.imgvideo;

import android.content.Context;
import android.opengl.GLES20;
import android.util.Log;

import com.jiaquan.livepusher.R;
import com.jiaquan.livepusher.egl.WLEGLSurfaceView;
import com.jiaquan.livepusher.egl.WLShaderUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class WLImgVideoRender implements WLEGLSurfaceView.WLGLRender {
    private Context context;

    private final float[] vertexData = {//顶点坐标
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
            1f, 1f,
    };

    private final float[] fragmentData = {//FBO坐标
            0f, 0f,//FBO坐标
            1f, 0f,
            0f, 1f,
            1f, 1f

//            0f, 1f,//纹理坐标
//            1f, 1f,
//            0f, 0f,
//            1f, 0f
            //纹理图像旋转操作
//            1f, 0f,
//            0f, 0f,
//            1f, 1f,
//            0f, 1f
    };

    private FloatBuffer vertexBuffer;
    private FloatBuffer fragmentBuffer;

    private int program;
    private int vPosition;
    private int fPosition;
    private int textureid;
    private int sTexture;

    private int vboId;
    private int fboId;

    private int imgTextureId;

    private OnRenderCreateListener onRenderCreateListener;

    public void setOnRenderCreateListener(OnRenderCreateListener onRenderCreateListener) {
        this.onRenderCreateListener = onRenderCreateListener;
    }

    public interface OnRenderCreateListener {
        void onCreate(int textid);
    }

    private WLImgFboRender wlImgFboRender;

    private int srcImg = 0;

    public WLImgVideoRender(Context context) {
        this.context = context;

        wlImgFboRender = new WLImgFboRender(context);

        vertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);
        vertexBuffer.position(0);

        fragmentBuffer = ByteBuffer.allocateDirect(fragmentData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(fragmentData);
        fragmentBuffer.position(0);
    }

    @Override
    public void onSurfaceCreated() {
        wlImgFboRender.onCreate();

        String vertexSource = WLShaderUtil.readRawTxt(context, R.raw.vertex_shader_screen);
        String fragmentSource = WLShaderUtil.readRawTxt(context, R.raw.fragment_shader_screen);

        program = WLShaderUtil.createProgram(vertexSource, fragmentSource);

        vPosition = GLES20.glGetAttribLocation(program, "v_Position");
        fPosition = GLES20.glGetAttribLocation(program, "f_Position");
        sTexture = GLES20.glGetUniformLocation(program, "sTexture");
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        int[] vbos = new int[1];
        GLES20.glGenBuffers(1, vbos, 0);
        vboId = vbos[0];

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);
        GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4 + fragmentData.length * 4, null, GLES20.GL_STATIC_DRAW);
        GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, vertexData.length * 4, vertexBuffer);
        GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4, fragmentData.length * 4, fragmentBuffer);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        int[] fbos = new int[1];
        GLES20.glGenBuffers(1, fbos, 0);
        fboId = fbos[0];
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);

        int[] textureIds = new int[1];
        GLES20.glGenTextures(1, textureIds, 0);
        textureid = textureIds[0];

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glUniform1i(sTexture, 0);

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, width, height, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, textureid, 0);
        if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
            Log.e("WLImgVideoRender", "fbo wrong");
        } else {
            Log.e("WLImgVideoRender", "fbo success");
        }

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

        if (onRenderCreateListener != null) {
            onRenderCreateListener.onCreate(textureid);
        }

        GLES20.glViewport(0, 0, width, height);
        wlImgFboRender.onChange(width, height);
    }

    @Override
    public void onDrawFrame() {
        imgTextureId = WLShaderUtil.loadTexture(srcImg, context);

        Log.d("WLImgVideoRender", "imgTextureId is : " + imgTextureId);
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(1f, 0f, 0f, 1f);

        GLES20.glUseProgram(program);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, imgTextureId);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);

        GLES20.glEnableVertexAttribArray(vPosition);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8,
                0);

        GLES20.glEnableVertexAttribArray(fPosition);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8,
                vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        int[] ids = new int[]{imgTextureId};
        GLES20.glDeleteTextures(1, ids, 0);

        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

        wlImgFboRender.onDraw(textureid);
    }

    public void setCurrentImgSrc(int src) {
        srcImg = src;
//        imgTextureId = WlShaderUtil.loadTexrute(src, context);
    }
}
