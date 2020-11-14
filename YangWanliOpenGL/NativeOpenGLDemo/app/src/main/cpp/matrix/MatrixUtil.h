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

#endif //NATIVEOPENGLDEMO_MATRIXUTIL_H
