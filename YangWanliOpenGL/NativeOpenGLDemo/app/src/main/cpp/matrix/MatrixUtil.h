//
// Created by jiaqu on 2020/11/14.
//

#ifndef NATIVEOPENGLDEMO_MATRIXUTIL_H
#define NATIVEOPENGLDEMO_MATRIXUTIL_H

static void initMatrix(float *matrix){
    for (int i = 0; i < 16; ++i) {
        if (i % 5 == 0){
            matrix[i] = 1;
        }else{
            matrix[i] = 0;
        }
    }
}

//2D图像旋转都是沿着Z轴旋转
//旋转矩阵，这里沿着Z轴旋转  第一个参数是角度，第二个参数是单位矩阵
static void rotateMatrix(double angle, float* matrix){
    angle = angle *(M_PI/180.0);//将角度转为弧度

    //修改矩阵中的部分特定值，表示沿着Z轴旋转
    matrix[0] = cos(angle);
    matrix[1] = -sin(angle);
    matrix[4] = sin(angle);
    matrix[5] = cos(angle);
}

/*2D缩放
缩放一般是均匀缩放
一般是对X轴和Y轴同值缩放，Z轴默认是1
*/
static void scaleMatrix(double scale, float* matrix){
    matrix[0] = scale;
    matrix[5] = scale;
}

/**
 * 2D平移
 *只需修改X轴和Y轴即可
 */
 static void transMatrix(double x, double y, float* matrix){
     matrix[3] = x;
     matrix[7] = y;
 }

 /**
  * 正交投影
  */
  static void orthoM(float left, float right, float bottom, float top, float* matrix){
      matrix[0] = 2/(right-left);
      matrix[3] = (right + left)/(right - left) * -1;
      matrix[5] = 2 / (top - bottom);
      matrix[7] = (top + bottom) / (top - bottom) * -1;
      matrix[10] = 1;
      matrix[11] = 1;
  }
#endif //NATIVEOPENGLDEMO_MATRIXUTIL_H
