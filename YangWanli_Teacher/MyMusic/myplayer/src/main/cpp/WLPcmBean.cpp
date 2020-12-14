//
// Created by jiaqu on 2020/12/14.
//
#include "WLPcmBean.h"

WLPcmBean::WLPcmBean(SAMPLETYPE *buffer, int size) {
    this->buffer = (char *) malloc(size);
    this->buffsize = size;
    memcpy(this->buffer, buffer, size);
};

WLPcmBean::~WLPcmBean() {
    free(buffer);
    buffer = NULL;
}