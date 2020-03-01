//
// Created by 祖国瑞 on 2020-02-15.
//

#include "PicPreviewer.h"
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <android/log.h>

using namespace std;
using namespace chrono;

#define MODULE_NAME "PicTexture"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)

static void threadCallback(void *context) {
    PicPreviewer* picPreviewer = (PicPreviewer *)context;
    picPreviewer->renderLoop();
}

PicPreviewer::PicPreviewer() {
    LOGD("PicPreviewer init");
    previewSurface = EGL_NO_SURFACE;

    width = 720;
    height = 1024;
    msgQueue.clear();

}

PicPreviewer::~PicPreviewer() {
    LOGD("PicPreviewer delete");
}

bool PicPreviewer::start() {
    LOGD("start enter");
//    message = NONE;
    renderThread = new thread(threadCallback, this);
    LOGD("start exit");
    return true;

}

bool PicPreviewer::stop() {
    LOGD("stop");
    if(renderThread == NULL)
    {
        return true;
    }
    unique_lock<mutex> locker(renderMu);
//    message = EXIT;
    msgQueue.push_back(RenderThreadMessage::EXIT);
    locker.unlock();
    newMsgSignal.notify_all();
    renderThread->join();
    return true;
}



void PicPreviewer::resetSize(int width, int height) {

    unique_lock<mutex> locker(renderMu);
    LOGD("resetSize enter");
    msgQueue.push_back(RenderThreadMessage::RESET_SIZE);
    this->width = width;
    this->height = height;
    newMsgSignal.notify_all();
    locker.unlock();
    LOGD("resetSize exit");
}

void PicPreviewer::setWindow(ANativeWindow *window) {
    LOGD("set window enter");
    this->window = window;
    unique_lock<mutex> locker(renderMu);
    msgQueue.push_back(RenderThreadMessage::SET_WINDOW);
    newMsgSignal.notify_all();
    locker.unlock();
    LOGD("set window exit");
}

void PicPreviewer::renderLoop() {
    LOGD("render loop enter");

    bool exitFlag = false;

    RenderThreadMessage message;
    while(!exitFlag)
    {
        LOGD("render loop start");

        unique_lock<mutex> msgLock(renderMu);
        while(msgQueue.size() == 0)
        {
            newMsgSignal.wait(msgLock);
        }
        message = msgQueue.front();
        msgQueue.pop_front();
        switch (message)
        {
            case SET_WINDOW:
                init();
                break;
            case EXIT:
                exitFlag = true;
                break;
            case RESET_SIZE:
                picRender->resetRenderSize(0, 0, width, height);
                break;
            default:
                break;
        }
        if(eglCore)
        {
            eglCore->makeCurrent(previewSurface);
            drawFrame();
        }


        msgLock.unlock();
        LOGD("render loop over");

    }
}

bool PicPreviewer::init() {
    LOGD("init");
    eglCore = new EGLCore();
    eglCore->init();
    previewSurface = eglCore->createWindowSurface(window);
    eglCore->makeCurrent(previewSurface);

    picTexture = new PicTexture();

    if(!(picTexture->createTexture()))
    {
        LOGE("create texture failed");
        destroy();
        return false;
    }

    updateTexImage();

    picRender = new PicRender();
    if(!picRender->init(width, height, picTexture))
    {
        LOGE("init render failed");
        destroy();
        return false;
    }
    return true;
}

void PicPreviewer::destroy() {
    LOGD("destroy");
    if(eglCore)
    {
        eglCore->releaseSurface(previewSurface);
        eglCore->release();
        delete(eglCore);
        eglCore = NULL;
    }
}

void PicPreviewer::updateTexImage() {
    LOGD("updateTexImage");
    int width = 500;
    int height = 500;
    uint8_t *pixel = (uint8_t *)malloc(width * height * 4 * sizeof(uint8_t));
    for(int i = 0; i < width * height; i++)
    {
        pixel[4 * i] = 0xff;
        pixel[4 * i + 1] = 0xff;
        pixel[4 * i + 2] = 0x00;
        pixel[4 * i + 3] = 0xff;
    }
    picTexture->updateDataToTexture(pixel, width, height);

}

void PicPreviewer::drawFrame() {
    LOGD("drawFrame");
    picRender->render();
    if(!eglCore->swapBuffers(previewSurface))
    {
        LOGE("swap buffers failed");
    }
}




