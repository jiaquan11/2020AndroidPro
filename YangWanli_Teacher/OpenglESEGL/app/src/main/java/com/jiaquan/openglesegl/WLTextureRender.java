package com.jiaquan.openglesegl;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class WLTextureRender implements WLEGLSurfaceView.WLGLRender {
    private Context context;

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
//        0f, 1f,
//        1f, 1f,
//        0f, 0f,
//        1f,0f

            //纹理图像旋转操作
            1f, 0f,
            0f, 0f,
            1f, 1f,
            0f, 1f
    };


    private FloatBuffer vertexBuffer;
    private FloatBuffer textureBuffer;

    private int program;
    private int vPosition;
    private int fPosition;
    private int sTexture;
    //    private int afColor;
    private int textureid;

    private Bitmap bitmap = null;

    private int vboId;

    public WLTextureRender(Context context) {
        this.context = context;

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
    }

    @Override
    public void onSurfaceCreated() {
        String vertexSource = WLShaderUtil.readRawTxt(context, R.raw.vertex_shader);
        String fragmentSource = WLShaderUtil.readRawTxt(context, R.raw.fragment_shader);
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
            GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4 + textureData.length * 4, null,
                    GLES20.GL_STATIC_DRAW);

            //4.为VBO设置顶点数据的值
            GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, 0, vertexData.length * 4, vertexBuffer);
            GLES20.glBufferSubData(GLES20.GL_ARRAY_BUFFER, vertexData.length * 4, textureData.length * 4, textureBuffer);

            //5.解绑VBO
            GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
            Log.i("WLTextureRender", "vertexData.length: " + vertexData.length);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            int[] textureIds = new int[1];
            GLES20.glGenTextures(1, textureIds, 0);
            if (textureIds[0] == 0) {
                return;
            }
            textureid = textureIds[0];

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

            bitmap = BitmapFactory.decodeResource(context.getResources(), R.drawable.mingren);
            if (bitmap == null) {
                Log.e("WLTextureRender", "bitmap decodeResource error!");
                return;
            }

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        }
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        GLES20.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame() {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);//白色清屏

        GLES20.glUseProgram(program);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glUniform1i(sTexture, 0);

        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);
        GLES20.glEnableVertexAttribArray(vPosition);
//        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, 0);

        GLES20.glEnableVertexAttribArray(fPosition);
//        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, vertexData.length*4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
    }
}
