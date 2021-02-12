package com.jiaquan.livepusher.camera;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;

import com.jiaquan.livepusher.R;
import com.jiaquan.livepusher.egl.WLEGLSurfaceView;
import com.jiaquan.livepusher.egl.WLShaderUtil;
import com.jiaquan.livepusher.util.DisplayUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class WLCameraRender implements WLEGLSurfaceView.WLGLRender, SurfaceTexture.OnFrameAvailableListener {
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
    private int fboTextureid;
    private int cameraTextureid;

    private int umatrix;
    private float[] matrix = new float[16];

    private int vboId;
    private int fboId;

    private int screenWidth;
    private int screenHeight;

    private int width;
    private int height;

    private WLCameraFboRender wlCameraFboRender;

    private SurfaceTexture surfaceTexture;

    private OnSurfaceCreateListener onSurfaceCreateListener;

    public void setOnSurfaceCreateListener(OnSurfaceCreateListener onSurfaceCreateListener) {
        this.onSurfaceCreateListener = onSurfaceCreateListener;
    }

    public interface OnSurfaceCreateListener {
        void onSurfaceCreate(SurfaceTexture surfaceTexture, int textureid);
    }

    public WLCameraRender(Context context) {
        this.context = context;

        //获取到屏幕宽高
        screenWidth = DisplayUtil.getScreenWidth(context);
        screenHeight = DisplayUtil.getScreenHeight(context);

        wlCameraFboRender = new WLCameraFboRender(context);

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
        Log.i("WLCameraRender", "onSurfaceCreated");
        wlCameraFboRender.onCreate();

        String vertexSource = WLShaderUtil.readRawTxt(context, R.raw.vertex_shader);
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
            fboTextureid = textureIds[0];

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, fboTextureid);

//            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
//            GLES20.glUniform1i(sTexture, 0);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

            //3.设置FBO分配内存大小
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, screenWidth, screenHeight, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
            //////////////////////////////////////////////

            //4.将纹理绑定到FBO
            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, fboTextureid, 0);

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

            ///////////////////////////////////////////////////////////////////////////
            int[] textureIdsoes = new int[1];
            GLES20.glGenTextures(1, textureIdsoes, 0);
            cameraTextureid = textureIdsoes[0];

            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, cameraTextureid);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glUniform1i(sTexture, 0);

            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);

            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

            surfaceTexture = new SurfaceTexture(cameraTextureid);
            surfaceTexture.setOnFrameAvailableListener(this);

            if (onSurfaceCreateListener != null) {
                onSurfaceCreateListener.onSurfaceCreate(surfaceTexture, fboTextureid);
            }
            GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, 0);
        }
    }

    public void resetMatrix() {
        Matrix.setIdentityM(matrix, 0);
    }

    public void setAngle(float angle, float x, float y, float z) {
        Matrix.rotateM(matrix, 0, angle, x, y, z);
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        Log.i("WLCameraRender", "onSurfaceChanged width: " + width + " height: " + height);
//        wlCameraFboRender.onChange(width, height);
//
//        GLES20.glViewport(0, 0, width, height);

        this.width = width;
        this.height = height;
    }

    @Override
    public void onDrawFrame() {

        surfaceTexture.updateTexImage();

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

        GLES20.glUseProgram(program);

        //在FBO中使用屏幕宽高进行渲染
        GLES20.glViewport(0, 0, screenWidth, screenHeight);

        GLES20.glUniformMatrix4fv(umatrix, 1, false, matrix, 0);

        //绑定使用FBO
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboId);

        //绑定使用VBO
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vboId);

        GLES20.glEnableVertexAttribArray(vPosition);
//        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 8, 0);

        GLES20.glEnableVertexAttribArray(fPosition);
//        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);
        GLES20.glVertexAttribPointer(fPosition, 2, GLES20.GL_FLOAT, false, 8, vertexData.length * 4);

        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

        //解绑纹理
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0);
        //解绑VBO
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        //解绑FBO
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);

        //FBO处理完成后，绘制到窗口时，使用实际的窗口大小进行显示
        wlCameraFboRender.onChange(width, height);
        //将FBO输出纹理id绘制到窗口
        wlCameraFboRender.onDraw(fboTextureid);
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

    }

    public int getFboTextureid() {
        return fboTextureid;
    }
}
