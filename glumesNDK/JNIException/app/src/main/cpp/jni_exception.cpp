//
// Created by jiaqu on 2020/10/31.
//

#include "base.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jniexception_JNIException_nativeInvokeJavaException(
        JNIEnv* env,
        jobject instance){
    jclass cls = env->FindClass("com/jiaquan/jniexception/JNIException");
    jmethodID mid = env->GetMethodID(cls, "operation", "()I");
    jmethodID mid2 = env->GetMethodID(cls, "<init>", "()V");

    jobject obj = env->NewObject(cls, mid2);
    env->CallIntMethod(obj, mid);

    jthrowable exc = env->ExceptionOccurred();
    if (exc){
        env->ExceptionDescribe();//描述异常
        env->ExceptionClear();//清除异常，不至于崩溃
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jniexception_JNIException_nativeThrowException(
        JNIEnv* env,
        jobject instance){
    jclass cls = env->FindClass("java/lang/IllegalArgumentException");
    env->ThrowNew(cls, "native throw exception");
}