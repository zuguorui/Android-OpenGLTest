//
// Created by zu on 2024/7/26.
//

#pragma once

#include <stdlib.h>
#include <iostream>
#include <thread>
#include <android/native_window.h>
#include "LinkedBlockingQueue.h"
#include "EGLWindow.h"

typedef std::function<void(EGLWindow&)> GLTask;

class GLEnv {
public:
    GLEnv();
    GLEnv(const GLEnv& src) = delete;
    GLEnv(GLEnv&& src) = delete;
    ~GLEnv();

    bool start(void *window);
    void run(GLTask& task);
    void stop();
    void setSize(int width, int height);

private:

    int screenWidth = 1080;
    int screenHeight = 1920;

    ANativeWindow *window = nullptr;

    static void runCallback(void *context);
    void runLoop();

    EGLWindow eglWindow;
    std::thread *runThread = nullptr;
    std::atomic<bool> stopFlag = std::atomic<bool>(false);
    LinkedBlockingQueue<GLTask> taskQueue = LinkedBlockingQueue<GLTask>();
};

