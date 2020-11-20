#include <jni.h>
#include <string>

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_accessInstanceField(
        JNIEnv* env,
        jobject instance, jobject animal) {
        jclass cls = env->GetObjectClass(animal);
        jfieldID fid = env->GetFieldID(cls, "name", "Ljava/lang/String;");
        jstring str = env->NewStringUTF("this is new name");
        env->SetObjectField(animal, fid, str);//更改Java类中非静态字段的值
}

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_accessStaticField(
        JNIEnv* env,
        jobject instance, jobject animal) {
    jclass cls = env->GetObjectClass(animal);
    jfieldID fid = env->GetStaticFieldID(cls, "num", "I");
    int num = env->GetStaticIntField(cls, fid);
    env->SetStaticIntField(cls, fid, ++num);//更改Java类中静态字段的值
}

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_staticAccessInstanceField(
        JNIEnv* env,
        jclass clazz) {
    jfieldID fid = env->GetStaticFieldID(clazz, "num", "I");
    int num = env->GetStaticIntField(clazz, fid);
    env->SetStaticIntField(clazz, fid, ++num);
}

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_accessInstanceMethod(
        JNIEnv* env,
        jobject instance, jobject animal) {
    jclass cls = env->GetObjectClass(animal);
    jmethodID mid = env->GetMethodID(cls, "callInstanceMethod", "(I)V");
    env->CallVoidMethod(animal, mid, 2);
}

extern "C" JNIEXPORT void JNICALL
Java_com_jiaquan_jniaccessfield_JNIAccessField_accessStaticMethod(
        JNIEnv* env,
        jobject instance, jobject animal) {
    jclass cls = env->GetObjectClass(animal);
    jmethodID mid = env->GetStaticMethodID(cls, "callStaticMethod", "(Ljava/lang/String;)Ljava/lang/String;");

    jstring str = env->NewStringUTF("jstring");
    env->CallStaticObjectMethod(cls, mid, str);

    mid = env->GetStaticMethodID(cls, "callStaticMethod", "([Ljava/lang/String;I)Ljava/lang/String;");
    jclass strClass = env->FindClass("java/lang/String");
    int size = 2;
    jobjectArray strArray = env->NewObjectArray(size, strClass, nullptr);
    jstring strItem;
    for (int i = 0; i < size; i++){
        strItem = env->NewStringUTF("string in native");
        env->SetObjectArrayElement(strArray, i, strItem);
    }

    env->CallStaticObjectMethod(cls, mid, strArray, size);
}