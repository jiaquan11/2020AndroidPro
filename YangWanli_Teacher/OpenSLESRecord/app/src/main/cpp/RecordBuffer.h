//
// Created by jiaqu on 2021/2/13.
//

#ifndef OPENSLESRECORD_RECORDBUFFER_H
#define OPENSLESRECORD_RECORDBUFFER_H

class RecordBuffer {
public:
    RecordBuffer(int buffersize);

    ~RecordBuffer();

    short *getRecordBuffer();

    short *getNowBuffer();

public:
    short **buffer;
    int index = -1;
};

#endif //OPENSLESRECORD_RECORDBUFFER_H
