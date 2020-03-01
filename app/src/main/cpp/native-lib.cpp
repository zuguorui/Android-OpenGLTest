#include <jni.h>
#include <string>
#include <android/log.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "PicPreviewer.h"

using namespace std;

#define MODULE_NAME "native_lib"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)


PicPreviewer *picPreviewer = NULL;

ANativeWindow *window = NULL;




extern "C" JNIEXPORT jstring JNICALL
Java_com_example_opengltest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT bool JNICALL
Java_com_example_opengltest_MainActivity_nInit(JNIEnv *env, jobject instance)
{
    picPreviewer = new PicPreviewer();
    return picPreviewer->start();
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_opengltest_MainActivity_nSetSurface(JNIEnv *env, jobject instance, jobject surface)
{
    if(surface != NULL && picPreviewer != NULL)
    {
        window = ANativeWindow_fromSurface(env, surface);
        picPreviewer->setWindow(window);
    }else if(window != NULL){
        ANativeWindow_release(window);
        window = NULL;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_opengltest_MainActivity_nResetSize(JNIEnv *env, jobject instance, jint width, jint height)
{
    if(picPreviewer != NULL)
    {
        picPreviewer->resetSize(width, height);
    }
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_opengltest_MainActivity_nStop(JNIEnv *env, jobject instance)
{
    if(picPreviewer != NULL)
    {
        picPreviewer->stop();
    }
}
