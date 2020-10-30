//
// Created by jiaqu on 2020/10/30.
//

#include "base.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_jiaquan_jniconstructorclass_JNIConstructorClass_invokeAnimalConstructors(
        JNIEnv* env,
        jobject instance) {
    jclass cls = env->FindClass("com/jiaquan/jniconstructorclass/Animal");
    jmethodID mid = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
    jstring str = env->NewStringUTF("this is animal name");
    jobject animal = env->NewObject(cls, mid, str);
    return animal;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_jiaquan_jniconstructorclass_JNIConstructorClass_allocObjectConstructor(
        JNIEnv* env,
        jobject instance) {
    jclass cls = env->FindClass("com/jiaquan/jniconstructorclass/Animal");
    jmethodID mid = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
    jobject animal = env->AllocObject(cls);
    jstring str = env->NewStringUTF("this is animal name22");
    env->CallNonvirtualVoidMethod(animal, cls, mid, str);
    return animal;
}