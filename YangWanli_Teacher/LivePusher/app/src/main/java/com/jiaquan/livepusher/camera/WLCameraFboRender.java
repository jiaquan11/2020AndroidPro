package com.jiaquan.livepusher.camera;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;

import com.jiaquan.livepusher.R;
import com.jiaquan.livepusher.egl.WLShaderUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class WLCameraFboRender {
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

            0f, 0f,
            0f, 0f,
            0f, 0f,
            0f, 0f
    };

    private final float[] fragmentData = {//纹理坐标
            0f, 1f,
            1f, 1f,
            0f, 0f,
            1f, 0f

//            //纹理图像旋转操作
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
    private int sTexture;

    private int vboId;

    private Bitmap bitmap;
    private int bitmapTextureId;

    public WLCameraFboRender(Context context) {
        this.context = context;

        bitmap = WLShaderUtil.createTextImage("视频直播和推流:jiaquan", 50, "#ff0000", "#00000000", 0);

        float r = 1.0f * bitmap.getWidth() / bitmap.getHeight();
        float w = r * 0.1f;//相当于正交投影，图片等比例
        Log.i("WLCameraFboRender", "w is " + w);

        vertexData[8] = 0.8f - w;//左下角坐标
        vertexData[9] = -0.8f;

        vertexData[10] = 0.8f;//右下角坐标
        vertexData[11] = -0.8f;

        vertexData[12] = 0.8f - w;//左上角
        vertexData[13] = -0.7f;

        vertexData[14] = 0.8f;//右上角
        vertexData[15] = -0.7f;

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

    public void onCreate() {
        //开启透明功能，如果开启此功能，设置有透明就会透明，否则默认是黑色
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);

        String vertexSource = WLShaderUtil.readRawTxt(context, R.raw.vertex_shader_screen);
        String fragmentSource = WLShaderUtil.readRawTxt(context, R.raw.fragment_shader_screen);

        program = WLShaderUtil.createProgram(vertexSource, fragmentSource);
        if (program > 0) {
            vPosition = GLES20.glGetAttribLocation(program, "v_Position");
            fPosition = GLES20.glGetAttribLocation(program, "f_Position");
            sTexture = GLES20.glGetUniformLocation(program, "sTexture");

            ////////////////////////////////////////////////////////////////////////////////
            //VBO
            //1.创建VBO
            int[] vbos = new int[1];
            GLES20.glGenBuffers(1, vbos, 0);
            vboId = vbos[0];
            //2.绑定VBO
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);
            //3.分配VBO需要的缓存大小
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4 + fragmentData.length * 4, null,
                    GLES20.GL_STATIC_DRAW);

            //4.为VBO设置顶点数据的值
            GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, vertexData.length * 4, vertexBuffer);
            GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4, fragmentData.length * 4, fragmentBuffer);

            //5.解绑VBO
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
            Log.i("WLTextureRender", "vertexData.length: " + vertexData.length);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            bitmapTextureId = WLShaderUtil.loadBitmapTexture(bitmap);
        }
    }

    public void onChange(int width, int height) {
        GLES20.glViewport(0, 0, width, height);
    }

    public void onDraw(int textureId) {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(1.0f, 0.0f, 0.0f, 1.0f);//白色清屏

        GLES20.glUseProgram(program);

        //绑定使用VBO
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);

        //fbo
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glUniform1i(sTexture, 0);

        GLES20.glEnableVertexAttribArray(vPosition);
//        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, 0);

        GLES20.glEnableVertexAttribArray(fPosition);
//        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);


        //bitmap 水印图片渲染
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, bitmapTextureId);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glUniform1i(sTexture, 0);

        GLES20.glEnableVertexAttribArray(vPosition);
//        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, 32);

        GLES20.glEnableVertexAttribArray(fPosition);
//        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        //解绑纹理id
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        //解绑VBO
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
    }
}
