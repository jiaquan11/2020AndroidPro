//
// Created by jiaqu on 2020/10/30.
//
#include "base.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_jiaquan_jnireference_JNIReference_errorCacheLocalReference(
        JNIEnv* env,
        jobject instance) {
    jclass localRefs = env->FindClass("java/lang/String");//局部引用
    jmethodID mid = env->GetMethodID(localRefs, "<init>", "(Ljava/lang/String;)V");
    jstring str = env->NewStringUTF("string");

    for (int i = 0; i < 1000; ++i) {
        jclass cls = env->FindClass("java/lang/String");
        env->DeleteLocalRef(cls);
    }
    return static_cast<jstring>(env->NewObject(localRefs, mid, str));
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_jiaquan_jnireference_JNIReference_cacheWithGlobalReference(
        JNIEnv* env,
        jobject instance) {
    static jclass stringClass = nullptr;
    if (stringClass == nullptr){
        jclass cls = env->FindClass("java/lang/String");
        stringClass = static_cast<jclass>(env->NewGlobalRef(cls));//全局引用
        env->DeleteLocalRef(cls);
    }else{
        LOGI("use cached");
    }

    jmethodID mid = env->GetMethodID(stringClass, "<init>", "(Ljava/lang/String;)V");
    jstring str = env->NewStringUTF("string");
    return static_cast<jstring>(env->NewObject(stringClass, mid, str));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnireference_JNIReference_useWeakGlobalReference(
        JNIEnv* env,
        jobject instance) {
    static jclass stringClass = nullptr;
    if (stringClass == nullptr){
        jclass cls = env->FindClass("java/lang/String");
        stringClass = static_cast<jclass>(env->NewWeakGlobalRef(cls));//弱引用
        env->DeleteLocalRef(cls);
    }else{
        LOGI("use cached");
    }

    jmethodID mid = env->GetMethodID(stringClass, "<init>", "(Ljava/lang/String;)V");
    jboolean isGc = env->IsSameObject(stringClass, nullptr);
}