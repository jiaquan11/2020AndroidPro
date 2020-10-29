#include "base.h"
#include <string>
#include "jvm.h"
#include <pthread.h>

void *threadCallback(void *);

static jclass threadClazz;
static jmethodID threadMethod;
static jobject threadObject;

#define JAVA_CLASS "com/jiaquan/jniinvokemethod/JNIInvokeMethod"

void nativeCallback(
        JNIEnv* env,
        jobject instance, jobject callback) {
    LOGI("nativeCallback");
    jclass callbackClazz = env->GetObjectClass(callback);
    jmethodID callbackMethod = env->GetMethodID(callbackClazz, "callback", "()V");
    env->CallVoidMethod(callback, callbackMethod);
}

void nativeThreadCallback(
        JNIEnv* env,
        jobject instance, jobject callback) {
    threadObject = env->NewGlobalRef(callback);
    threadClazz = env->GetObjectClass(callback);
    threadMethod = env->GetMethodID(threadClazz, "callback", "()V");

    pthread_t handle;
    pthread_create(&handle, nullptr, threadCallback, nullptr);
}

void *threadCallback(void *){
    JavaVM* gVM = getJvm();
    JNIEnv* env = nullptr;
    if (gVM->AttachCurrentThread(&env, nullptr) == 0){
        LOGI("threadCallback AttachCurrentThread");
        env->CallVoidMethod(threadObject, threadMethod);
        LOGI("threadCallback AttachCurrentThread 222");
        gVM->DetachCurrentThread();
    }
    return 0;
}

static JNINativeMethod gMethods[] = {
        {"nativeCallback", "(Lcom/jiaquan/jniinvokemethod/ICallbackMethod;)V", (void*)nativeCallback},
        {"nativeThreadCallback", "(Lcom/jiaquan/jniinvokemethod/IThreadCallback;)V", (void*)threadCallback}
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

    setJvm(vm);

    registerNativeMethods(env, JAVA_CLASS, gMethods, 2);

    LOGI("jni onLoad call");
    return JNI_VERSION_1_6;
}
