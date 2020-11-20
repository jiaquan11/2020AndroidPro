//
// Created by jiaqu on 2020/10/27.
//
#include <base.h>
//#include <jvm.h>

#define JAVA_CLASS "com/jiaquan/jnidynamicload/JNIDynamicLoad"

jstring getMessage(JNIEnv* env, jobject jobj){
    return env->NewStringUTF("this is msg");
}

jint plus(JNIEnv* env, jobject jobj, int x, int y){
    return x + y;
}

static JNINativeMethod gMethods[] = {
        {"getNativeString", "()Ljava/lang/String;", (void*)getMessage},
        {"sum", "(II)I", (void*)plus}
};

int registerNativeMethods(JNIEnv* env, const char* name,
                          JNINativeMethod* methods, jint nMethods){
    jclass jcls;
    jcls = env->FindClass(name);
    if (jcls == nullptr){
        return JNI_FALSE;
    }

    if (env->RegisterNatives(jcls, methods, nMethods) < 0){
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

JNIEXPORT int JNICALL JNI_OnLoad(JavaVM *vm, void* reserved){
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK){
        return JNI_FALSE;
    }

    registerNativeMethods(env, JAVA_CLASS, gMethods, 2);

    LOGI("jni onLoad call");
    return JNI_VERSION_1_6;
}