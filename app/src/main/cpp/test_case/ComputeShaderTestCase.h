//
// Created by zu on 2024/7/26.
//

#pragma once
#include "TestCase.h"

class ComputeShaderTestCase: public TestCase{

public:
    ComputeShaderTestCase();

    ~ComputeShaderTestCase();

    void stop() override;

protected:
    GLTask onCreateTask() override;

    void taskFunc(EGLWindow &eglWindow);

    std::atomic_bool stopFlag = false;
    std::mutex exitMu;
    std::condition_variable exitCond;
};
