//
// Created by jiaqu on 2020/11/25.
//

#include "CallJava.h"

CallJava::CallJava(JavaVM *vm, JNIEnv *env, jobject obj) {
    javaVm = vm;
    jniEnv = env;
    jobj = env->NewGlobalRef(obj);

    jclass clz = jniEnv->GetObjectClass(jobj);
    if (!clz) {
        if (LOG_DEBUG) {
            LOGE("get jclass wrong");
        }
        return;
    }

    jmid_prepared = env->GetMethodID(clz, "onCallPrepared", "()V");
    jmid_load = env->GetMethodID(clz, "onCallLoad", "(Z)V");
    jmid_timeinfo = env->GetMethodID(clz, "onCallTimeInfo", "(II)V");
    jmid_error = env->GetMethodID(clz, "onCallError", "(ILjava/lang/String;)V");
    jmid_complete = env->GetMethodID(clz, "onCallComplete", "()V");
    jmid_volumeDB = env->GetMethodID(clz, "onCallVolumeDB", "(I)V");
    jmid_pcmtoaac = env->GetMethodID(clz, "encodePcmToAAC", "([BI)V");
    jmid_pcminfo = env->GetMethodID(clz, "onCallPcmInfo", "([BI)V");
    jmid_pcmrate = env->GetMethodID(clz, "onCallPcmRate", "(III)V");
}

CallJava::~CallJava() {

}

void CallJava::onCallPrepared(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_prepared);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_prepared);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallLoad(int type, bool load) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_load, load);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallTimeInfo(int type, int curr, int total) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        env->CallVoidMethod(jobj, jmid_timeinfo, curr, total);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallError(int type, int code, char *msg) {
    if (type == MAIN_THREAD) {
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }
        jstring jmsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jobj, jmid_error, code, jmsg);
        env->DeleteLocalRef(jmsg);
        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallComplete(int type) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_complete);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        env->CallVoidMethod(jobj, jmid_complete);

        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallVolumeDB(int type, int db) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_volumeDB, db);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        env->CallVoidMethod(jobj, jmid_volumeDB, db);

        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallPcmToAAC(int type, void *buffer, int size) {
    if (type == MAIN_THREAD) {
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        jniEnv->CallVoidMethod(jobj, jmid_pcmtoaac, jbuffer, size);
        jniEnv->DeleteLocalRef(jbuffer);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        jbyteArray jbuffer = env->NewByteArray(size);
        env->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        env->CallVoidMethod(jobj, jmid_pcmtoaac, jbuffer, size);
        env->DeleteLocalRef(jbuffer);

        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallPcmInfo(int type, void *buffer, int size) {
    if (type == MAIN_THREAD) {
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        jniEnv->CallVoidMethod(jobj, jmid_pcminfo, jbuffer, size);
        jniEnv->DeleteLocalRef(jbuffer);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        jbyteArray jbuffer = env->NewByteArray(size);
        env->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        env->CallVoidMethod(jobj, jmid_pcminfo, jbuffer, size);
        env->DeleteLocalRef(jbuffer);

        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallPcmRate(int type, int samplerate, int bit, int channels) {
    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_pcmrate, samplerate, bit, channels);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        env->CallVoidMethod(jobj, jmid_pcmrate, samplerate, bit, channels);

        javaVm->DetachCurrentThread();
    }
}
