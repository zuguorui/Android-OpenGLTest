
#include "ComputeProgram.h"
#include <cstdio>
#include <sstream>
#include <istream>
#include <fstream>
#include <stdio.h>

#include "Log.h"

#define TAG "ComputeProgram"

using namespace std;

ComputeProgram::ComputeProgram() {

}

ComputeProgram::ComputeProgram(const char *shaderCode) {
    compile(shaderCode);
}

ComputeProgram::~ComputeProgram() {
    if (ID > 0) {
        glDeleteProgram(ID);
        ID = 0;
    }
}

void ComputeProgram::use() {
    glUseProgram(ID);
}

bool ComputeProgram::isReady() {
    return ID > 0;
}

bool ComputeProgram::compile(const char *shaderCode) {

    GLuint shader;
    GLint success;
    char infoLog[512];

    try {
        shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &shaderCode, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            printf("Error when compile compute shader: %s\n", infoLog);
            return false;
        }

        ID = glCreateProgram();
        glAttachShader(ID, shader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            glGetProgramInfoLog(ID, 512, nullptr, infoLog);
            printf("Error when link compute program: %s\n", infoLog);
            ID = 0;
            return false;
        }

        glDeleteShader(shader);
    } catch (const char *msg) {
        LOGE(TAG, "%s", msg);
        release();
    }
    if (shader > 0) {
        glDeleteShader(shader);
    }
    return ID > 0;
}

void ComputeProgram::release() {
    if (ID > 0) {
        glDeleteProgram(ID);
    }
    ID = 0;
}

void ComputeProgram::setBool(const std::string &name, bool value) const {
    if (ID > 0) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
}

void ComputeProgram::setInt(const std::string &name, int value) const {
    if (ID > 0) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
}

void ComputeProgram::setUInt(const std::string &name, unsigned int value) const {
    if (ID > 0) {
        glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
    }
}

void ComputeProgram::setFloat(const std::string &name, float value) const {
    if (ID > 0) {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
}

void ComputeProgram::setVec2(const std::string &name, float x, float y) {
    if (ID > 0) {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
}

void ComputeProgram::setVec2(const std::string &name, const float *value) {
    if (ID > 0) {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
}

void ComputeProgram::setVec3(const std::string &name, float x, float y, float z) {
    if (ID > 0) {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
}

void ComputeProgram::setVec3(const std::string &name, const float *value) {
    if (ID > 0) {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
}

void ComputeProgram::setVec4(const std::string &name, float x, float y, float z, float w) {
    if (ID > 0) {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
}

void ComputeProgram::setVec4(const std::string &name, const float *value) {
    if (ID > 0) {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value);
    }
}
