//
// Created by jiaqu on 2020/10/27.
//

#ifndef JNIINVOKEMETHOD_JVM_H
#define JNIINVOKEMETHOD_JVM_H
#include "base.h"

#ifdef __cplusplus
extern "C"{
#endif

void setJvm(JavaVM *jvm);

JavaVM *getJvm();

#ifdef __cplusplus
}
#endif

#endif //JNIINVOKEMETHOD_JVM_H
