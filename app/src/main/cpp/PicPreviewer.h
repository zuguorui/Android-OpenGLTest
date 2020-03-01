//
// Created by 祖国瑞 on 2020-02-15.
//

#ifndef OPENGLTEST_PICPREVIEWER_H
#define OPENGLTEST_PICPREVIEWER_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <list>
#include "EGLCore.h"
#include "PicRender.h"
#include "PicTexture.h"


using namespace std;

class PicPreviewer {
public:
    PicPreviewer();
    ~PicPreviewer();
    bool start();
    bool stop();
    void setWindow(ANativeWindow *window);
    void resetSize(int width, int height);

    void renderLoop();

private:
    PicTexture *picTexture;
    PicRender *picRender;
    EGLCore *eglCore;
    EGLSurface previewSurface;

    int width;
    int height;
    enum RenderThreadMessage{
        NONE = 0, SET_WINDOW, RESET_SIZE, EXIT
    };

    thread *renderThread = NULL;
    mutex renderMu;
    condition_variable newMsgSignal;

//    RenderThreadMessage message = NONE;

    ANativeWindow *window;

//    static void *threadCallback(void *context);



    bool init();

    void updateTexImage();

    void destroy();

    void drawFrame();

    list<RenderThreadMessage> msgQueue;

};


#endif //OPENGLTEST_PICPREVIEWER_H
