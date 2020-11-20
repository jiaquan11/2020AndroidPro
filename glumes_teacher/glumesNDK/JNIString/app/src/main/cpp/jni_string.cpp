
#include <string>
#include <base.h>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_jiaquan_jnistring_JNIString_callNativeString(JNIEnv* env, jobject thiz, jstring jstr){
    const char* str = env->GetStringUTFChars(jstr, 0);
    LOGI("java string is %s", str);
    env->ReleaseStringUTFChars(jstr, str);

    return env->NewStringUTF("this is C style string");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnistring_JNIString_stringMethod(JNIEnv* env, jobject thiz, jstring jstr){
    const char* str = env->GetStringUTFChars(jstr, 0);

    char buf[128];
    int len = env->GetStringLength(jstr);
    LOGI("java string length is %d", len);

    env->GetStringUTFRegion(jstr, 0, len-1, buf);

    LOGI("jstring is %s", buf);
    env->ReleaseStringUTFChars(jstr, str);
}