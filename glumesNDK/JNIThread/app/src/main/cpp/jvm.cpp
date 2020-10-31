//
// Created by jiaqu on 2020/10/27.
//

#include "jvm.h"
#include "base.h"

static JavaVM* gVM = nullptr;

#ifdef __cplusplus
extern "C"{
#endif

void setJvm(JavaVM *jvm){
    gVM = jvm;
}

JavaVM *getJvm(){
    return gVM;
}

#ifdef __cplusplus
}
#endif