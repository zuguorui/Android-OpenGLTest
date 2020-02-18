//
// Created by 祖国瑞 on 2020-02-14.
//

#include "PicTexture.h"
#include <android/log.h>

#define MODULE_NAME "PicTexture"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, MODULE_NAME, __VA_ARGS__)

PicTexture::PicTexture() {

}

PicTexture::~PicTexture() {

}

bool PicTexture::createTexture() {

    int ret = initTexture();
    if(ret < 0)
    {
        LOGE("init texture failed");
        dealloc();
        return false;
    }
    return true;
}

int PicTexture::initTexture() {
    texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    return 1;
}

void PicTexture::updateDataToTexture(uint8_t *pixels, int width, int height) {
    if(pixels)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        if(glGetError())
        {
            LOGE("glBindTexture error");
            return;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    }
}

bool PicTexture::bindTexture(GLint uniformSampler) {
    glActiveTexture(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    if(glGetError())
    {
        LOGE("glBindTexture error");
        return false;
    }
    glUniform1i(uniformSampler, 0);
}

void PicTexture::dealloc() {
    if(texture)
    {
        glDeleteTextures(1, &texture);
    }
}
