//
// Created by zu on 2024/7/31.
//

#pragma once
#include "TestCase.h"

/**
 * 测试使用OpenGL ES渲染各种格式的纹理。例如rgb565、nv21。
 * 能否直接渲染关键在于OpenGL ES是否支持相应格式的纹理。glTexImage2D有三个关键参数：
 * internalFormat: 描述纹理的分量格式
 * format: 描述像素格式
 * type: 描述数据的格式
 * 这三个参数有对应关系，并不是任何组合都可以渲染。具体组合可以查看
 * https://registry.khronos.org/OpenGL-Refpages/es3/中glTexImage2D的参数说明。
 *
 * 例如如果渲染rgb8数据，三个参数可以指定为GL_RGB8、GL_RGB、GL_UNSIGNED_BYTE。
 * 渲染rgb565数据，是GL_RGB565、GL_RGB、GL_UNSIGNED_BYTE或GL_UNSIGNED_SHORT_5_6_5。
 *
 * 渲染yuv数据则更复杂一点。通用的一点是先把yuv数据看作是rgb数据创建纹理，能保证颜色分量可以单独被
 * OpenGL ES做采样，最后在着色器中做yuv到rgb的转换。
 *
 * 对于planner格式，例如420p，三个分量各有自己的平面，那么可以选择使用GL_R8、GL_RED、GL_UNSIGNED_BYTE
 * 的参数组合来分别为yuv创建三个纹理。注意三个纹理的尺寸。
 *
 * 对于semi-planner格式，例如nv21，可以先试用GL_R8、GL_RED、GL_UNSIGNED_BYTE为y分量创建纹理。
 * 然后用GL_RG8、GL_RG、GL_UNSIGNED_BYTE来为uv分量创建一个共用纹理。
 *
 * 对于packed格式，例如YUYV422，则无法靠这种方式直接去创建相应纹理，只能先把各通道分离开。具体怎么分离，
 * 除了传统的方式，也可以选择速度更快的计算着色器（可以参考ComputeShaderTest）。
 *
 * 但是以上仅限于色彩深度为8bit的情况。大体来看，OpenGL ES可以渲染的像素格式基本是16bit float、4bit
 * 或8bit int，以及少见的如RGB565、RGB10_A2、RGB5_A1这几种按位拼接的像素格式。对于拼接的类型，目前还没测试
 * 大尾序是否可以正常渲染，或者有其他的方法可以设置此项。
 * */
class RenderTest: public TestCase{
public:
    RenderTest();
    ~RenderTest();
protected:
    GLTask onCreateTask() override;

    void testFunc_rgb565(EGLWindow &eglWindow);
    void testFunc_nv21(EGLWindow &eglWindow);
    void testFunc_yuv420p(EGLWindow &eglWindow);
    void testFunc_yuv422p(EGLWindow &eglWindow);

    // 强行直接渲染yuyv422数据，但是会导致画面模糊。具体原因查看render_yuyv422.frag里的注释。
    // 最好的方式还是先把通道数据分开再单独创建纹理。
    void testFunc_yuyv422(EGLWindow &eglWindow);
};


