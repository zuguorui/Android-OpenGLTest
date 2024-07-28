//
// Created by zu on 2024/7/28.
//

#ifndef OPENGLTEST_INITTEST_H
#define OPENGLTEST_INITTEST_H


#include "TestCase.h"

class InitTest: public TestCase {
public:
    InitTest();
    ~InitTest();

protected:
    GLTask onCreateTask() override;
    void taskFunc(EGLWindow &eglWindow);
};


#endif //OPENGLTEST_INITTEST_H
