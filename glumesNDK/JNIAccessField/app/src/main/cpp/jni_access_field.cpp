#include <jni.h>
#include <string>

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_accessInstanceField(
        JNIEnv* env,
        jobject instance, jobject animal) {
        jclass cls = env->GetObjectClass(animal);
        jfieldID fid = env->GetFieldID(cls, "name", "Ljava/lang/String;");
        jstring str = env->NewStringUTF("this is new name");
        env->SetObjectField(animal, fid, str);
}

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_accessStaticField(
        JNIEnv* env,
        jobject instance, jobject animal) {
    jclass cls = env->GetObjectClass(animal);
    jfieldID fid = env->GetStaticFieldID(cls, "num", "I");
    int num = env->GetStaticIntField(cls, fid);
    env->SetStaticIntField(cls, fid, ++num);
}

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_staticAccessInstanceField(
        JNIEnv* env,
        jclass clazz) {
    jfieldID fid = env->GetStaticFieldID(clazz, "num", "I");
    int num = env->GetStaticIntField(clazz, fid);
    env->SetStaticIntField(clazz, fid, ++num);
}