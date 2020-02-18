//
// Created by 祖国瑞 on 2020-02-15.
//

#include "PicPreviewer.h"
#include <android/log.h>

#define MODULE_NAME "PicTexture"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)

static void threadCallback(void *context) {
    PicPreviewer* picPreviewer = (PicPreviewer *)context;
    picPreviewer->renderLoop();
}

PicPreviewer::PicPreviewer() {
    LOGD("PicPreviewer init");
    message = NONE;
    previewSurface = EGL_NO_SURFACE;

    width = 720;
    height = 720;
}

PicPreviewer::~PicPreviewer() {
    LOGD("PicPreviewer delete");
}

bool PicPreviewer::start() {
    LOGD("start");
    message = NONE;
    renderThread = new thread(threadCallback, this);

}

bool PicPreviewer::stop() {
    LOGD("stop");
    if(renderThread == NULL)
    {
        return true;
    }
    unique_lock<mutex> locker(renderMu);
    message = EXIT;
    locker.unlock();
    newMsgSignal.notify_all();
    renderThread->join();
    return true;
}



void PicPreviewer::resetSize(int width, int height) {
    LOGD("resetSize");
    unique_lock<mutex> locker(renderMu);
    this->width = width;
    this->height = height;
    picRender->resetRenderSize(0, 0, width, height);
    locker.unlock();
    newMsgSignal.notify_all();
}

void PicPreviewer::setWindow(ANativeWindow *window) {
    LOGD("set window");
    this->window = window;
    renderMu.lock();
    message = SET_WINDOW;
    renderMu.unlock();
    newMsgSignal.notify_all();
}

void PicPreviewer::renderLoop() {
    LOGD("render loop enter");
    bool exit = false;


    while(!exit)
    {
        unique_lock<mutex> msgLock(renderMu);
        switch (message)
        {
            case SET_WINDOW:
                init();
                break;
            case EXIT:
                exit = true;
                break;
            default:
                break;
        }
        message = NONE;
        if(eglCore)
        {
            eglCore->makeCurrent(previewSurface);
            drawFrame();
        }
        newMsgSignal.wait(msgLock);

        msgLock.unlock();

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
        pixel[4 * i + 1] = 0x00;
        pixel[4 * i + 2] = 0x00;
        pixel[4 * i + 3] = 0x88;
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




