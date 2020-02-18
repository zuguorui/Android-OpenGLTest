//
// Created by 祖国瑞 on 2020-02-14.
//

#ifndef OPENGLTEST_PICTEXTURE_H
#define OPENGLTEST_PICTEXTURE_H

#include <stdlib.h>
#include <stdio.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class PicTexture {
public:
    PicTexture();
    ~PicTexture();

    bool createTexture();
    void updateDataToTexture(uint8_t *pixels, int width, int height);
    bool bindTexture(GLint uniformSampler);
    void dealloc();

private:
    GLuint texture = 0;

    int initTexture();

};


#endif //OPENGLTEST_PICTEXTURE_H
