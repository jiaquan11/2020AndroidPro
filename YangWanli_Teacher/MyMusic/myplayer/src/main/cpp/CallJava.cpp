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
    jmid_renderyuv = env->GetMethodID(clz, "onCallRenderYUV", "(II[B[B[B)V");
    jmid_supportvideo = env->GetMethodID(clz, "onCallIsSupportMediaCodec", "(Ljava/lang/String;)Z");
    jmid_initmediacodec = env->GetMethodID(clz, "onCallinitMediaCodec", "(Ljava/lang/String;II[B[B)V");
    jmid_decodeVPacket = env->GetMethodID(clz, "onCallDecodeVPacket", "(I[B)V");
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

void
CallJava::onCallRenderYUV(int type, int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv) {
    if (type == MAIN_THREAD) {
        jbyteArray y = jniEnv->NewByteArray(width*height);
        jniEnv->SetByteArrayRegion(y, 0, width*height, reinterpret_cast<const jbyte *>(fy));

        jbyteArray u = jniEnv->NewByteArray(width*height/4);
        jniEnv->SetByteArrayRegion(u, 0, width*height/4, reinterpret_cast<const jbyte *>(fu));

        jbyteArray v = jniEnv->NewByteArray(width*height/4);
        jniEnv->SetByteArrayRegion(v, 0, width*height/4, reinterpret_cast<const jbyte *>(fv));

        jniEnv->CallVoidMethod(jobj, jmid_renderyuv, width, height, y, u, v);

        jniEnv->DeleteLocalRef(y);
        jniEnv->DeleteLocalRef(u);
        jniEnv->DeleteLocalRef(v);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        jbyteArray y = env->NewByteArray(width*height);
        env->SetByteArrayRegion(y, 0, width*height, reinterpret_cast<const jbyte *>(fy));

        jbyteArray u = env->NewByteArray(width*height/4);
        env->SetByteArrayRegion(u, 0, width*height/4, reinterpret_cast<const jbyte *>(fu));

        jbyteArray v = env->NewByteArray(width*height/4);
        env->SetByteArrayRegion(v, 0, width*height/4, reinterpret_cast<const jbyte *>(fv));

        env->CallVoidMethod(jobj, jmid_renderyuv, width, height, y, u, v);

        env->DeleteLocalRef(y);
        env->DeleteLocalRef(u);
        env->DeleteLocalRef(v);

        javaVm->DetachCurrentThread();
    }
}

bool CallJava::onCallIsSupportVideo(int type, const char *ffcodecname) {
    bool support = false;
    if (type == MAIN_THREAD) {
        jstring type = jniEnv->NewStringUTF(ffcodecname);
        support = jniEnv->CallBooleanMethod(jobj, jmid_supportvideo, type);
        jniEnv->DeleteLocalRef(type);
        return support;
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return support;
            }
        }

        jstring type = env->NewStringUTF(ffcodecname);
        support = env->CallBooleanMethod(jobj, jmid_supportvideo, type);
        env->DeleteLocalRef(type);

        javaVm->DetachCurrentThread();

        return support;
    }
}

void CallJava::onCallinitMediaCodec(int type, const char* mime, int width, int height, int csd0_size, int csd1_size, uint8_t* csd_0, uint8_t* csd_1) {
    if (type == MAIN_THREAD) {
        jstring typejstr = jniEnv->NewStringUTF(mime);
       jbyteArray csd0 = jniEnv->NewByteArray(csd0_size);
       jniEnv->SetByteArrayRegion(csd0, 0, csd0_size, reinterpret_cast<const jbyte *>(csd_0));
        jbyteArray csd1 = jniEnv->NewByteArray(csd1_size);
        jniEnv->SetByteArrayRegion(csd1, 0, csd1_size, reinterpret_cast<const jbyte *>(csd_1));

        jniEnv->CallVoidMethod(jobj, jmid_initmediacodec, typejstr, width, height, csd0, csd1);
        jniEnv->DeleteLocalRef(csd0);
        jniEnv->DeleteLocalRef(csd1);
        jniEnv->DeleteLocalRef(typejstr);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        jstring typejstr = env->NewStringUTF(mime);
        jbyteArray csd0 = env->NewByteArray(csd0_size);
        env->SetByteArrayRegion(csd0, 0, csd0_size, reinterpret_cast<const jbyte *>(csd_0));
        jbyteArray csd1 = env->NewByteArray(csd1_size);
        env->SetByteArrayRegion(csd1, 0, csd1_size, reinterpret_cast<const jbyte *>(csd_1));

        env->CallVoidMethod(jobj, jmid_initmediacodec, typejstr, width, height, csd0, csd1);
        env->DeleteLocalRef(csd0);
        env->DeleteLocalRef(csd1);
        env->DeleteLocalRef(typejstr);

        javaVm->DetachCurrentThread();
    }
}

void CallJava::onCallDecodeVPacket(int type, int datasize, uint8_t *packetdata) {
    if (type == MAIN_THREAD) {
        jbyteArray data = jniEnv->NewByteArray(datasize);
        jniEnv->SetByteArrayRegion(data, 0, datasize, reinterpret_cast<const jbyte *>(packetdata));
        jniEnv->CallVoidMethod(jobj, jmid_decodeVPacket, datasize, data);
        jniEnv->DeleteLocalRef(data);
    } else if (type == CHILD_THREAD) {
        JNIEnv *env;
        if (javaVm->AttachCurrentThread(&env, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv wrong");
                return;
            }
        }

        jbyteArray data = env->NewByteArray(datasize);
        env->SetByteArrayRegion(data, 0, datasize, reinterpret_cast<const jbyte *>(packetdata));
        env->CallVoidMethod(jobj, jmid_decodeVPacket, datasize, data);
        env->DeleteLocalRef(data);

        javaVm->DetachCurrentThread();
    }
}
