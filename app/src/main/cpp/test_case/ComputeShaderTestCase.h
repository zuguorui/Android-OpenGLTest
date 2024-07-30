//
// Created by zu on 2024/7/26.
//

#pragma once
#include "TestCase.h"

/**
 * 计算着色器的测试用例
 * */
class ComputeShaderTestCase: public TestCase{

public:
    ComputeShaderTestCase();

    ~ComputeShaderTestCase();

    void stop() override;

protected:
    GLTask onCreateTask() override;

    void testFunc(EGLWindow &eglWindow);

    /*
     * 这里演示了利用计算着色器将一些packed格式的图像重新排列像素，以方便送去GL进行渲染。
     * packed结尾的函数是将像素打包到一起，例如RGB565会重排列为RGBARGBA，nv21会重排列为
     * YUVXYUVX，单个元素都是8bit。这基本是利用了OpenGL ES纹理参数为GL_RGBA8进行渲染的。
     * 这样做的优点是无需在计算着色器中进行同步，可以最大化利用并行效率。缺点是每个像素都会浪费1byte空间。
     *
     * 也可以将每个色彩分量分离为单独一个buffer，这样片段着色器就需要3个纹理。优点是可以节省内存。缺点是
     * 由于计算着色器的输入输出只能是32bit的，每次读取或者写入都必须以32bit为最小单位。如果元素大小为8bit，
     * 那么就不得不在计算着色器中使用原子操作或者锁，这样就浪费了一些并行计算效率。如果不使用锁，就只能使用更大的数据类型
     * 来存储输出，这样反而比第一种方法更浪费。况且一般除了8bit之外，其他长度的类型OpenGL ES很可能无法渲染，限制
     * 还挺多。
     * */
    void testFunc_rgb565_packed(EGLWindow &eglWindow);

    void testFunc_nv21_packed(EGLWindow &eglWindow);

    /*
     * planner结尾的函数表示它将三个颜色分量分成三个buffer分别送到片段着色器。具体优劣可以看上面。
     * 这种情况下，纹理格式是GL_R8, GL_RED, GL_UNSIGNED_BYTE。
     * */
    void testFunc_nv21_planner(EGLWindow &eglWindow);

    /*
     * 测试精度。发现无论在计算着色器中设置何种精度，SSBO总是按32bit读写。
     * */
    void testFunc_precision(EGLWindow &eglWindow);

    std::atomic_bool stopFlag = false;
    std::mutex exitMu;
    std::condition_variable exitCond;
};

