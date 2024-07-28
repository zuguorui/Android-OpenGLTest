//
// Created by 祖国瑞 on 2022/12/6.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "GLES3/gl32.h"
#include "GLES3/gl3platform.h"

class RenderProgram {
public:
    RenderProgram();
    RenderProgram(const char *vertexShaderCode, const char *fragmentShaderCode);
    ~RenderProgram();

    bool compile(const char *vertexShaderCode, const char *fragmentShaderCode);

    bool isReady();

    void use();

    void release();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const float *value);
    void setVec2(const std::string &name, float x, float y);

    void setVec3(const std::string &name, const float *value);
    void setVec3(const std::string &name, float x, float y, float z);

    void setVec4(const std::string &name, const float *value);
    void setVec4(const std::string &name, float x, float y, float z, float w);

    GLuint getProgramID();

private:
    GLuint ID = 0;
};
