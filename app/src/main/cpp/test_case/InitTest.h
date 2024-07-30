//
// Created by zu on 2024/7/28.
//

#ifndef OPENGLTEST_INITTEST_H
#define OPENGLTEST_INITTEST_H


#include "TestCase.h"

/**
 * 测试OpenGL ES环境的初始化是否成功
 * */
class InitTest: public TestCase {
public:
    InitTest();
    ~InitTest();

protected:
    GLTask onCreateTask() override;
    void taskFunc(EGLWindow &eglWindow);
};


#endif //OPENGLTEST_INITTEST_H
