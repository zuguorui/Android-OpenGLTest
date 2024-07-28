//
// Created by zu on 2024/7/26.
//

#pragma once

#include "GLEnv.h"
#include <iostream>
#include <stdlib.h>
#include <android/asset_manager.h>

class TestCase {
public:
    ~TestCase() {
        if (window != nullptr) {
            ANativeWindow_release(window);
            window = nullptr;
        }
    }

    virtual void start(ANativeWindow *window, int width, int height, AAssetManager *assetManager) {
        this->assetManager = assetManager;
        this->window = window;
        this->screenWidth = width;
        this->screenHeight = height;
        glEnv.start(window);
        glEnv.setSize(width, height);
        GLTask task = onCreateTask();
        glEnv.run(task);
    };

    virtual void stop() {
        glEnv.stop();
        delete(assetManager);
        //delete(window);
    };

protected:
    GLEnv glEnv;
    AAssetManager *assetManager = nullptr;
    ANativeWindow *window;
    int screenWidth = 1920;
    int screenHeight = 1080;
    virtual GLTask onCreateTask() = 0;
};


