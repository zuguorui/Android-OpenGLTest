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

#define TAG "native-lib"

#define TEST_INIT 0
#define TEST_COMPUTE_SHADER 1

extern "C"
JNIEXPORT void JNICALL
Java_com_zu_opengltest_GLTest_stopTest(JNIEnv *env, jobject thiz) {

    LOGD(TAG, "stopTest");
    if (testCase == nullptr) {
        return;
    }
    testCase->stop();
    delete(testCase);
    testCase = nullptr;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_zu_opengltest_GLTest_startTest(JNIEnv *env, jobject thiz, jint testType, jobject surface,
                                        jint width, jint height, jobject asset_manager) {
    if (testCase != nullptr) {
        return false;
    }
    switch (testType) {
        case TEST_INIT:
            testCase = new InitTest();
            break;
        case TEST_COMPUTE_SHADER:
            testCase = new ComputeShaderTestCase();
            break;
        default:
            break;
    }
    if (testCase == nullptr) {
        return false;
    }
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    AAssetManager *assetManager = AAssetManager_fromJava(env, asset_manager);
    testCase->start(window, width, height, assetManager);
    return true;
}


