#include "base.h"
#include <string>

/*
 * |Java      |Native     |
 * boolean    |jboolean   |
 * byte       |jbyte      |
 * char       |jchar      |
 * short      |jshort     |
 * int        |jint       |
 * long       |jlong      |
 * float      |jfloat     |
 * double     |jdouble    |
 */

/*
 * |Java Reference    |Native     |Java Reference     |Native    |
 * |All objects       |jobject    |char[]             |jcharArray|
 * |java.lang.Class   |jclass     |short[]            |jshortArray|
 * |java.lang.String  |jstring    |int[]              |jintArray  |
 * |Object[]          |jobjectArray  |long[]          |jlongArray  |
 * |boolean[]         |jbooleanArray  |float[]      |jfloatArray |
 * |byte[]             |jbyteArray     |double[]       |jdoubleArray|
 * |java.lang.Throwable  |jthrowable
 */
extern "C" JNIEXPORT jstring JNICALL
Java_com_jiaquan_jnireferencetype_JNIReferenceType_callNativeStringArray(
        JNIEnv* env,
        jobject /* this */,
        jobjectArray str_array) {
        int len = env->GetArrayLength(str_array);
        LOGI("len is %d", len);

        jstring firstStr = static_cast<jstring>(env->GetObjectArrayElement(str_array, 0));
        const char* str = env->GetStringUTFChars(firstStr, 0);
        LOGI("first str is %s", str);
        env->ReleaseStringUTFChars(firstStr, str);

        return env->NewStringUTF(str);
}