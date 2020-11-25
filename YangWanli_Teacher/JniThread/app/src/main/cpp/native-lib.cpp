#include <jni.h>
#include <string>
#include "log/androidLog.h"

#include "pthread.h"

pthread_t thread;

void *normalCallBack(void *data) {
    LOGI("create normal thread from C++");
    pthread_exit(&thread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithread_ThreadDemo_normalThread(JNIEnv *env, jobject thiz) {
    // TODO: implement normalThread()
    pthread_create(&thread, NULL, normalCallBack, NULL);
}

#include "queue"
#include "unistd.h"
#include "JavaListener.h"

pthread_t product;
pthread_t customer;
pthread_mutex_t mutex;
pthread_cond_t cond;

std::queue<int> queue;
bool isExit = false;

void *productCallBack(void *data) {
    while (true) {
        pthread_mutex_lock(&mutex);
        queue.push(1);
        pthread_cond_signal(&cond);
        LOGI("生产者生产一个产品，通知消费者消费，产品数量为%d", queue.size());
        pthread_mutex_unlock(&mutex);

        sleep(5);
    }
    pthread_exit(&product);
}

void *customerCallBack(void *data) {
    while (true) {
        pthread_mutex_lock(&mutex);
        if (queue.size() > 0) {
            queue.pop();
            LOGI("消费者消费一个产品，产品数量还剩余%d", queue.size());
        } else {
            LOGI("没有产品可以消费， 等待中....");
            pthread_cond_wait(&cond, &mutex);//等待过程中会自动解锁，然后其它线程可以调度使用
        }
        pthread_mutex_unlock(&mutex);

        usleep(1000 * 500);
    }
    pthread_exit(&customer);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithread_ThreadDemo_mutexThread(JNIEnv *env, jobject thiz) {
    // TODO: implement mutexThread()
    for (int i = 0; i < 10; ++i) {
        queue.push(i);
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&product, NULL, productCallBack, NULL);
    pthread_create(&customer, NULL, customerCallBack, NULL);
}

JavaVM *javaVm;
JavaListener *javaListener;
pthread_t childThread;

void *childCallback(void *data) {
    JavaListener *listener = (JavaListener *) (data);
    listener->onError(0, 101, "C++ call java method from child thread");
    pthread_exit(&childThread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_jiaquan_jnithread_ThreadDemo_callBackFromC(JNIEnv *env, jobject thiz) {
    // TODO: implement callBackFromC()
    javaListener = new JavaListener(javaVm, env, env->NewGlobalRef(thiz));
//    javaListener->onError(1, 100, "C++ call java method from main thread");

    pthread_create(&childThread, NULL, childCallback, javaListener);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    javaVm = vm;
    if (javaVm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}