//
// Created by jiaqu on 2020/11/15.
//

#include "FilterOne.h"

FilterOne::FilterOne() {
    initMatrix(matrix);//初始化为单位矩阵
}

FilterOne::~FilterOne() {

}

void FilterOne::onCreate() {
    vertexStr = GET_STR(
            attribute vec4 v_Position;
            attribute vec2 f_Position;
            varying vec2 ft_Position;
            uniform mat4 u_Matrix;
            void main() {
                ft_Position = f_Position;
                gl_Position = v_Position * u_Matrix;//将矩阵与顶点坐标进行相乘，用于图像旋转
            });

    fragmentStr = GET_STR(
            precision mediump float;
            varying vec2 ft_Position;
            uniform sampler2D sTexture;
            void main() {//texture2D表示GPU将输入得图像纹理像素进行读取，读取到GPU的管线中,最后渲染出来
                gl_FragColor = texture2D(sTexture, ft_Position);
            });

    program = createProgram(vertexStr, fragmentStr, &vShader, &fShader);
    LOGI("FilterOne callback_SurfaceCreate GET_STR opengl program: %d", program);

    //获取着色器程序中的这个变量a_position，返回一个变量id，用于给这个变量赋值
    vPosition = glGetAttribLocation(program, "v_Position");//顶点坐标变量
    fPosition = glGetAttribLocation(program, "f_Position");//纹理坐标变量
    sampler = glGetUniformLocation(program, "sTexture");//2D纹理变量
    u_matrix = glGetUniformLocation(program, "u_Matrix");//着色器矩阵变量

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);//绑定纹理

    LOGI("FilterOne textureID is %d", textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);//解绑纹理
    LOGI("FilterOne::onCreate end");
}

//指定屏幕显示界面的宽高
void FilterOne::onChange(int width, int height) {
    LOGI("FilterOne::onChange in");
    surface_width = width;
    surface_height = height;
    glViewport(0, 0, width, height);
    LOGI("FilterOne::onChange end");
}

void FilterOne::onDraw() {
    LOGI("FilterOne::onDraw in");
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);//指定刷屏颜色  1:不透明  0：透明
    glClear(GL_COLOR_BUFFER_BIT);//将刷屏颜色进行刷屏，但此时仍然处于后台缓冲中，需要swapBuffers交换到前台界面显示

    glUseProgram(program);//使用着色器程序

    //渲染时纹理赋值操作
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(sampler, 0);//GL_TEXTURE0表示就是第一层纹理

    glUniformMatrix4fv(u_matrix, 1, GL_FALSE, matrix);//给矩阵变量赋值

    if (pixels != NULL) {//为后台缓存显存中设置图片数据
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }

    //渲染时顶点赋值操作
    glEnableVertexAttribArray(vPosition);//使能这个着色器变量

    /*给着色器的顶点顶点变量赋值
     * 第一个参数是着色器的变量id,第二个参数是每个顶点两个值，第三个参数是值的类型，第四个参数表示是否
    归一化，已经有顶点参数，无需自动归一化。第五个参数表示每个顶点的跨度(这里每个顶点跨8个字节)，
     第六个参数表示顶点数组
     */
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, false, 8, vertexs);

    glEnableVertexAttribArray(fPosition);
    glVertexAttribPointer(fPosition, 2, GL_FLOAT, false, 8, fragments);
    /*opengl绘制
     * 绘制三角形，第二个参数表示从索引0开始，绘制三个顶点
     */
//    glDrawArrays(GL_TRIANGLES, 0, 3);//绘制三角形
//    glDrawArrays(GL_TRIANGLES, 0, 6);//绘制四边形
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//绘制四边形

    glBindTexture(GL_TEXTURE_2D, 0);

    LOGI("FilterOne::onDraw end");
}

void FilterOne::destroy() {
    glDeleteTextures(1, &textureID);
    glDetachShader(program, vShader);
    glDetachShader(program, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteProgram(program);
}

void FilterOne::setMatrix(int width, int height) {
    LOGI("FilterOne::setMatrix in");
//    initMatrix(matrix);
    //这里是矩阵投影操作
    //屏幕720*1280 图片:517*685
    float screen_r = 1.0 * width / height;
    float picture_r = 1.0 * w / h;
    if (screen_r > picture_r) {//图片宽度缩放
        LOGI("pic scale width");
        float r = width / (1.0 * height / h * w);
        LOGI("pic scale width r: %f", r);
        orthoM(-r, r, -1, 1, matrix);
    } else {//图片宽的比率大于屏幕，则宽进行直接覆盖屏幕，而图片高度缩放
        LOGI("pic scale height");
        float r = height / (1.0 * width / w * h);
        LOGI("pic scale height r: %f", r);
        orthoM(-1, 1, -r, r, matrix);
    }
    LOGI("FilterOne::setMatrix end");
}

void FilterOne::setPixel(void *data, int width, int height, int length) {
    LOGI("FilterOne::setPixel in");
    w = width;//图片宽
    h = height;//图片高
    pixels = data;
    if ((surface_width > 0) && (surface_height > 0)) {
        setMatrix(surface_width, surface_height);
    }
    LOGI("FilterOne::setPixel end");
}

void FilterOne::destroySource() {
    if (pixels != NULL) {
        pixels = NULL;
    }
}

