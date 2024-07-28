//
// Created by zu on 2024/7/26.
//

#include "GLEnv.h"
#include "Log.h"
#include "GLES3/gl32.h"
#include "GLES3/gl3platform.h"

#define TAG "GLEnv"

using namespace std;

GLEnv::GLEnv() {

}

GLEnv::~GLEnv() {
    stop();
}


bool GLEnv::start(void *window) {
    if (window == nullptr) {
        return false;
    }
    this->window = (ANativeWindow *)window;

    if (runThread != nullptr) {
        return false;
    }
    stopFlag = false;
    taskQueue.setBlockPop(true);
    runThread = new thread(runCallback, this);
    return true;
}

void GLEnv::stop() {
    if (runThread == nullptr) {
        return;
    }
    stopFlag = true;
    taskQueue.setBlockPop(false);
    if (runThread->joinable()) {
        runThread->join();
    }
    taskQueue.setBlockPop(true);
    delete(runThread);
    runThread = nullptr;
}

void GLEnv::setSize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    GLTask task = [&](EGLWindow &eglWindow) {
        glViewport(0, 0, screenWidth, screenHeight);
    };
    run(task);
}

void GLEnv::run(GLTask &task) {
    taskQueue.pushBack(task);
}

void GLEnv::runCallback(void *context) {
    ((GLEnv *)context)->runLoop();
}

void GLEnv::runLoop() {

    if (eglWindow.isReady()) {
        return;
    }
    if (!eglWindow.create(window)) {
        LOGE(TAG, "egl create failed");
        return;
    }
    if (!eglWindow.isReady()) {
        LOGE(TAG, "egl is not ready");
        return;
    }

    while (!stopFlag) {
        optional<GLTask> opt = taskQueue.popFront();
        if (opt.has_value()) {
            GLTask task = opt.value();
            task(eglWindow);
        }
    }

    eglWindow.release();
}





