//
// Created by zu on 2024/7/26.
//

#pragma once

#include "GLEnv.h"
#include <iostream>
#include <stdlib.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "Log.h"

class TestCase {
public:
    virtual ~TestCase() {
        LOGD(TAG, "~TestCase: begin");
        if (window != nullptr) {
            LOGI(TAG, "release native window");
            ANativeWindow_release(window);
            window = nullptr;
        }
        LOGD(TAG, "~TestCase: complete");
    }

    virtual void start(ANativeWindow *window, int width, int height, AAssetManager *assetManager) {
        LOGD(TAG, "start: begin");
        this->assetManager = assetManager;
        this->window = window;
        this->screenWidth = width;
        this->screenHeight = height;
        glEnv.start(window);
        glEnv.setSize(width, height);
        GLTask task = onCreateTask();
        glEnv.run(task);
        LOGD(TAG, "start: complete");
    };

    virtual void stop() {
        LOGD(TAG, "stop: begin");
        glEnv.stop();
//        delete(assetManager);
        LOGD(TAG, "stop: complete");
    };

protected:
    GLEnv glEnv;
    AAssetManager *assetManager = nullptr;
    ANativeWindow *window;
    int screenWidth = 1920;
    int screenHeight = 1080;
    virtual GLTask onCreateTask() = 0;

private:
    const char *TAG = "TestCase";
};


