package com.jiaquan.openglesegl;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.opengl.Matrix;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class WLTextureRender implements WLEGLSurfaceView.WLGLRender {
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

            -0.5f, -0.5f,
            0.5f, -0.5f,
            -0.5f, 0.5f,
            0.5f, 0.5f
    };

    private final float[] fragmentData = {//FBO坐标
//            0f, 0f,//FBO坐标
//            1f, 0f,
//            0f, 1f,
//            1f, 1f,

            0f, 1f,//纹理坐标
            1f, 1f,
            0f, 0f,
            1f, 0f
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
    private int sTexture;
    private int textureid;
    private int umatrix;
    private float[] matrix = new float[16];

    private int vboId;
    private int fboId;

    private int imgTextureId;
    private int imgTextureId2;

    private FboRender fboRender;

    private int width;
    private int height;

    private OnRenderCreateListener onRenderCreateListener = null;
    public void setOnRenderCreateListener(OnRenderCreateListener onRenderCreateListener) {
        this.onRenderCreateListener = onRenderCreateListener;
    }

    public interface OnRenderCreateListener{
        void onCreate(int textid);
    }

    public WLTextureRender(Context context) {
        this.context = context;

        fboRender = new FboRender(context);

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
        fboRender.onCreate();

        String vertexSource = WLShaderUtil.readRawTxt(context, R.raw.vertex_shader_m);
        String fragmentSource = WLShaderUtil.readRawTxt(context, R.raw.fragment_shader);
        program = WLShaderUtil.createProgram(vertexSource, fragmentSource);
        if (program > 0) {
            vPosition = GLES20.glGetAttribLocation(program, "v_Position");
            fPosition = GLES20.glGetAttribLocation(program, "f_Position");
            sTexture = GLES20.glGetUniformLocation(program, "sTexture");
            umatrix = GLES20.glGetUniformLocation(program, "u_Matrix");

//////////////////////////////////////////////////////////////////////////////////////////////////////
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

            //////////////////////////////////////////////
            //FBO
            //1.创建FBO
            int[] fbos = new int[1];
            GLES20.glGenBuffers(1, fbos, 0);
            fboId = fbos[0];
            //2.绑定FBO
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);

            //纹理id生成操作
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

            //3.设置FBO分配内存大小
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, 1080, 2000, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
            //////////////////////////////////////////////

            //4.将纹理绑定到FBO
            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, textureid, 0);

            //5.检查FBO绑定是否成功
            if (GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER) != GLES20.GL_FRAMEBUFFER_COMPLETE) {
                Log.e("WLTextureRender", "fbo is wrong!!!");
            } else {
                Log.i("WLTextureRender", "fbo is successed!!!");
            }

//            bitmap = BitmapFactory.decodeResource(context.getResources(), R.drawable.mingren);
//            if (bitmap == null) {
//                Log.e("WLTextureRender", "bitmap decodeResource error!");
//                return;
//            }

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
            //解绑FBO
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

            imgTextureId = loadTexture(R.drawable.androids);
            imgTextureId2 = loadTexture(R.drawable.ghnl);

            if (onRenderCreateListener != null){
                onRenderCreateListener.onCreate(textureid);
            }
        }
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        Log.i("WLTextureRender", "onSurfaceChanged width: " + width + " height: " + height);
//        GLES20.glViewport(0, 0, width, height);
//
//        fboRender.onChange(width, height);

        this.width = width;
        this.height = height;

        width = 1080;
        height = 2000;

        if (width > height) {//横屏
            Matrix.orthoM(matrix, 0, -width / ((height / 702f) * 526f), width / ((height / 702f) * 526f), -1f, 1f, -1f, 1f);
        } else {//竖屏
            Matrix.orthoM(matrix, 0, -1, 1, -height / ((width / 526f) * 702f), height / ((width / 526f) * 702f), -1f, 1f);
        }

        Matrix.rotateM(matrix, 0, 180, 1, 0, 0);//沿着X轴旋转180度，即上下翻转
//        Matrix.rotateM(matrix, 0, 180, 0, 0, 1);//沿着Z轴旋转180度，即逆时针旋转180度
    }

    @Override
    public void onDrawFrame() {
        GLES20.glViewport(0, 0, 1080, 2000);

        //绑定FBO
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(1.0f, 0.0f, 0.0f, 1.0f);//hong色清屏  1.0f 1.0f 1.0f 1.0f表示白色 0.0f 0.0f 0.0f 1.0f表示黑色

        GLES20.glUseProgram(program);

        GLES20.glUniformMatrix4fv(umatrix, 1, false, matrix, 0);

        //绑定使用VBO
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);

        //绘制第一张纹理
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, imgTextureId);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glUniform1i(sTexture, 0);

        GLES20.glEnableVertexAttribArray(vPosition);
//        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, 0);

        GLES20.glEnableVertexAttribArray(fPosition);
//        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
//////////////////////////////////////////////////////////////////////////////////////////////////
        //绘制第二张纹理
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, imgTextureId2);
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glUniform1i(sTexture, 0);

        GLES20.glEnableVertexAttribArray(vPosition);
//        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, 32);

        GLES20.glEnableVertexAttribArray(fPosition);
//        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
/////////////////////////////////////////////////////////////////////////////////////////////

        //解绑纹理
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        //解绑VBO
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        GLES20.glViewport(0, 0, width, height);

//        //解绑FBO
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
//
        //解绑FBO后，会将FBO已绘制的结果输出到在之前已绑定的纹理id:textureid,并将整个结果进行窗口绘制出来
        fboRender.onDraw(textureid);
    }

    private int loadTexture(int src) {
        int[] textureIds = new int[1];
        GLES20.glGenTextures(1, textureIds, 0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureIds[0]);

        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), src);
        GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);

        return textureIds[0];
    }
}
