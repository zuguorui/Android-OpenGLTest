#include <jni.h>
#include <string>
#include "GLEnv.h"
#include "TestCase.h"
#include "ComputeShaderTestCase.h"
#include "InitTest.h"
#include <android/native_window.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

TestCase *testCase = nullptr;



extern "C"
JNIEXPORT void JNICALL
Java_com_zu_opengltest_GLTest_testComputeShader(JNIEnv *env, jobject thiz, jobject surface,
                                                jint width, jint height, jobject asset_manager) {
    if (testCase != nullptr) {
        return;
    }
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    AAssetManager *assetManager = AAssetManager_fromJava(env, asset_manager);
    testCase = new ComputeShaderTestCase();
    testCase->start(window, width, height, assetManager);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zu_opengltest_GLTest_stopTest(JNIEnv *env, jobject thiz) {
    if (testCase == nullptr) {
        return;
    }
    testCase->stop();
    delete(testCase);
    testCase = nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zu_opengltest_GLTest_testInit(JNIEnv *env, jobject thiz, jobject surface, jint width,
                                       jint height, jobject asset_manager) {
    if (testCase != nullptr) {
        return;
    }
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    AAssetManager *assetManager = AAssetManager_fromJava(env, asset_manager);
    testCase = new InitTest();
    testCase->start(window, width, height, assetManager);
}