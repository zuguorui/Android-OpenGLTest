#pragma once

#include <stdlib.h>
#include <iostream>

#include "GLES3/gl32.h"
#include "GLES3/gl3platform.h"

class ComputeProgram {
public:
    ComputeProgram();
    ComputeProgram(const char *shaderCode);
    ~ComputeProgram();

    void use();

    bool isReady();

    bool compile(const char *shaderPath);

    void release();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setUInt(const std::string &name, unsigned int value) const;

    void setVec2(const std::string &name, const float *value);
    void setVec2(const std::string &name, float x, float y);

    void setVec3(const std::string &name, const float *value);
    void setVec3(const std::string &name, float x, float y, float z);

    void setVec4(const std::string &name, const float *value);
    void setVec4(const std::string &name, float x, float y, float z, float w);

    GLuint getProgramID();
private:
    uint32_t ID = 0;
};