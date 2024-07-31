//
// Created by zu on 2024/7/26.
//

#include "ComputeShaderTestCase.h"
#include "utils.h"
#include "gl_utils.h"
#include "Log.h"
#include <GLES3/gl32.h>
#include <GLES3/gl3platform.h>
#include "ComputeProgram.h"
#include "RenderProgram.h"
#include "data.h"

#define TAG "ComputeShaderTestCase"

using namespace std;

ComputeShaderTestCase::ComputeShaderTestCase() {

}

ComputeShaderTestCase::~ComputeShaderTestCase() {
    LOGD(TAG, "~ComputeShaderTestCase: begin");
    LOGD(TAG, "~ComputeShaderTestCase: complete");
}

GLTask ComputeShaderTestCase::onCreateTask() {
    GLTask task = bind(&ComputeShaderTestCase::testFunc_nv21_packed, this, placeholders::_1);
    return task;
}

void ComputeShaderTestCase::testFunc_rgb565_packed(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *computeShaderCode = nullptr;
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;

    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 1536;
    int imageHeight = 864;
    int pixelCount = imageWidth * imageHeight;

    GLuint rawBuffer = 0, rgbBuffer = 0;
    GLuint rgbTex = 0;

    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
        if (computeShaderCode) {
            free(computeShaderCode);
        }
        if (renderVertCode) {
            free(renderVertCode);
        }
        if (renderFragCode) {
            free(renderFragCode);
        }
        if (imageData) {
            free(imageData);
        }
        if (rawBuffer) {
            glDeleteBuffers(1, &rawBuffer);
        }
        if (rgbBuffer) {
            glDeleteBuffers(1, &rgbBuffer);
        }


        if (rgbTex) {
            glDeleteTextures(1, &rgbTex);
        }

        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
        }

        if (EBO) {
            glDeleteBuffers(1, &EBO);
        }
    };


    computeShaderCode = load_shader_code(assetManager, "shaders/trans_rgb565_to_rgba8_packed.frag");
    if (computeShaderCode == nullptr) {
        LOGE(TAG, "load shader code failed");
        releaseResource();
        return;
    }

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_rgba8_packed.frag");
    if (renderFragCode == nullptr) {
        LOGE(TAG, "load render frag code failed");
        releaseResource();
        return;
    }


    if (!load_asset_file(assetManager, "images/output-1920x1080-rgb565.rgb", &imageData, &imageSize)) {
        LOGE(TAG, "load image failed");
        releaseResource();
        return;
    }

    //memset(imageData + imageSize / 2, 9, imageSize / 2);

    imageWidth = 1920;
    imageHeight = 1080;
    pixelCount = imageWidth * imageHeight;

    LOGD(TAG, "imageSize = %ld", imageSize);


    typedef uint32_t DATA_TYPE;
    int GL_INTERNAL_FORMAT = GL_RGBA8;
    int GL_FORMAT = GL_RGBA;
    int GL_DATA_TYPE = GL_UNSIGNED_BYTE;

    glGenBuffers(1, &rawBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, imageSize, imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // 申请三通道的buffer，用来存储分离后的通道数据。分离之后的数据都是uint8_t的

    glGenBuffers(1, &rgbBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rgbBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pixelCount * sizeof(DATA_TYPE), nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    ComputeProgram computeProgram;
    if (!computeProgram.compile(computeShaderCode)) {
        LOGE(TAG, "compile compute shader failed");
        releaseResource();
        return;
    }

    computeProgram.use();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, rawBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rgbBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rgbBuffer);

    computeProgram.setInt("width", imageWidth);
    computeProgram.setInt("height", imageHeight);

    glDispatchCompute(imageWidth / 16, imageHeight / 8, 1);

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);


    // 下个阶段，准备绘制

//    DATA_TYPE *tempBuffer1 = (DATA_TYPE *) malloc(pixelCount * sizeof(DATA_TYPE));
//    for (int i = 0; i < pixelCount; i++) {
//        tempBuffer1[i] = 0x0000FF00;
//    }
//
//    DATA_TYPE *tempBuffer0 = (DATA_TYPE *) malloc(pixelCount * sizeof(DATA_TYPE));
//    memset(tempBuffer0, 0, pixelCount * sizeof(DATA_TYPE));
//
//    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, rgbBuffer);
//    DATA_TYPE *redPtr = (DATA_TYPE *)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, pixelCount * sizeof(DATA_TYPE), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
//    LOGD(TAG, "redPtr = %ld", redPtr);
//
//    for (int i = 0; i < 32; i++) {
//        DATA_TYPE splitR = redPtr[i + 2 * imageWidth];
//        LOGD(TAG, "r = 0x%x", splitR);
//    }
//
//    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);


    // 分别构造rgb三个通道的texture
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, rgbBuffer);
    glGenTextures(1, &rgbTex);
    glBindTexture(GL_TEXTURE_2D, rgbTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth, imageHeight, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    float vertexData[20];
    compute_vertex(screenWidth, screenHeight, imageWidth, imageHeight, vertexData);
    create_vertex_objects(VAO, VBO, EBO, vertexData, EBO_indices);

    RenderProgram renderProgram;
    if (!renderProgram.compile(renderVertCode, renderFragCode)) {
        LOGE(TAG, "renderProgram compile failed");
        releaseResource();
        return;
    }

    renderProgram.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rgbTex);
    renderProgram.setInt("rgba_tex", 0);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();

    unique_lock<mutex> lock(exitMu);
    exitCond.wait(lock);

    releaseResource();
}

void ComputeShaderTestCase::testFunc_nv21_packed(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *computeShaderCode = nullptr;
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;

    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 1536;
    int imageHeight = 864;
    int pixelCount = imageWidth * imageHeight;

    // 原始图像的输入buffer
    GLuint yBuffer = 0, uvBuffer = 0;
    // 打包好的yuv输出buffer
    GLuint yuvBuffer = 0;
    GLuint yuvTex = 0;

    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
        if (computeShaderCode) {
            free(computeShaderCode);
        }
        if (renderVertCode) {
            free(renderVertCode);
        }
        if (renderFragCode) {
            free(renderFragCode);
        }
        if (imageData) {
            free(imageData);
        }
        if (yBuffer) {
            glDeleteBuffers(1, &yBuffer);
        }
        if (uvBuffer) {
            glDeleteBuffers(1, &uvBuffer);
        }
        if (yuvBuffer) {
            glDeleteBuffers(1, &yuvBuffer);
        }

        if (yuvTex) {
            glDeleteTextures(1, &yuvTex);
        }

        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
        }

        if (EBO) {
            glDeleteBuffers(1, &EBO);
        }
    };


    computeShaderCode = load_shader_code(assetManager, "shaders/trans_nv21_to_yuvx_packed.frag");
    if (computeShaderCode == nullptr) {
        LOGE(TAG, "load shader code failed");
        releaseResource();
        return;
    }

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_yuvx_packed.frag");
    if (renderFragCode == nullptr) {
        LOGE(TAG, "load render frag code failed");
        releaseResource();
        return;
    }


    if (!load_asset_file(assetManager, "images/output-1920x1080-nv21.yuv", &imageData, &imageSize)) {
        LOGE(TAG, "load image failed");
        releaseResource();
        return;
    }



    imageWidth = 1920;
    imageHeight = 1080;
    pixelCount = imageWidth * imageHeight;

    int64_t y_count = pixelCount;
    int64_t u_count = y_count / 4;
    int64_t v_count = y_count / 4;
    if (y_count + u_count + v_count != imageSize) {
        LOGE(TAG, "size not correct");
        releaseResource();
        return;
    }


    typedef uint32_t DATA_TYPE;
    int GL_INTERNAL_FORMAT = GL_RGBA8;
    int GL_FORMAT = GL_RGBA;
    int GL_DATA_TYPE = GL_UNSIGNED_BYTE;

    // 申请作为输入的yBuffer和uvBuffer
    glGenBuffers(1, &yBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, yBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pixelCount * sizeof(uint8_t), imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // nv21数据先Y，后VU。UV的总长度为总像素量的一半
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pixelCount / 2 * sizeof(uint8_t), imageData + pixelCount * sizeof(uint8_t), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // 申请作为输出的yuvBuffer
    glGenBuffers(1, &yuvBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, yuvBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pixelCount * sizeof(DATA_TYPE), nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    ComputeProgram computeProgram;
    if (!computeProgram.compile(computeShaderCode)) {
        LOGE(TAG, "compile compute shader failed");
        releaseResource();
        return;
    }

    computeProgram.use();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, yBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, yBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, uvBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, yuvBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, yuvBuffer);

    computeProgram.setInt("width", imageWidth);
    computeProgram.setInt("height", imageHeight);

    glDispatchCompute(imageWidth / 16, imageHeight / 8, 1);

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // 下个阶段，准备绘制
//    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yuvBuffer);
//    DATA_TYPE *ptr = (DATA_TYPE *)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, pixelCount * sizeof(DATA_TYPE), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
//
//    for (int i = 0; i < 16; i++) {
//        DATA_TYPE splitR = ptr[i];
//        LOGD(TAG, "r = 0x%x", splitR);
//    }
//    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
//    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);



    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yuvBuffer);
    glGenTextures(1, &yuvTex);
    glBindTexture(GL_TEXTURE_2D, yuvTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth, imageHeight, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    float vertexData[20];
    compute_vertex(screenWidth, screenHeight, imageWidth, imageHeight, vertexData);
    create_vertex_objects(VAO, VBO, EBO, vertexData, EBO_indices);

    RenderProgram renderProgram;
    if (!renderProgram.compile(renderVertCode, renderFragCode)) {
        LOGE(TAG, "renderProgram compile failed");
        releaseResource();
        return;
    }

    renderProgram.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yuvTex);
    renderProgram.setInt("yuvx_tex", 0);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();

    unique_lock<mutex> lock(exitMu);
    exitCond.wait(lock);

    releaseResource();
}

void ComputeShaderTestCase::testFunc_nv21_planner(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *computeShaderCode = nullptr;
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;

    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 1536;
    int imageHeight = 864;
    int pixelCount = imageWidth * imageHeight;

    // 原始图像的y平面buffer和uv平面buffer
    GLuint yBuffer = 0, uvBuffer = 0;
    // 分离后的uv buffer。
    GLuint vBuffer = 0, uBuffer = 0;

    GLuint yTex = 0, uTex = 0, vTex = 0;

    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
        if (computeShaderCode) {
            free(computeShaderCode);
        }
        if (renderVertCode) {
            free(renderVertCode);
        }
        if (renderFragCode) {
            free(renderFragCode);
        }
        if (imageData) {
            free(imageData);
        }
        if (yBuffer) {
            glDeleteBuffers(1, &yBuffer);
        }
        if (uvBuffer) {
            glDeleteBuffers(1, &uvBuffer);
        }
        if (vBuffer) {
            glDeleteBuffers(1, &vBuffer);
        }

        if (uBuffer) {
            glDeleteBuffers(1, &uBuffer);
        }

        if (yTex) {
            glDeleteTextures(1, &yTex);
        }

        if (uTex) {
            glDeleteTextures(1, &uTex);
        }

        if (vTex) {
            glDeleteTextures(1, &vTex);
        }

        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
        }

        if (EBO) {
            glDeleteBuffers(1, &EBO);
        }
    };

    computeShaderCode = load_shader_code(assetManager, "shaders/trans_nv21_to_yuv_planner.frag");
    if (computeShaderCode == nullptr) {
        LOGE(TAG, "load shader code failed");
        releaseResource();
        return;
    }

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_yuv_planner.frag");
    if (renderFragCode == nullptr) {
        LOGE(TAG, "load render frag code failed");
        releaseResource();
        return;
    }

    if (!load_asset_file(assetManager, "images/output-1920x1080-nv21.yuv", &imageData, &imageSize)) {
        LOGE(TAG, "load image failed");
        releaseResource();
        return;
    }

    imageWidth = 1920;
    imageHeight = 1080;
    pixelCount = imageWidth * imageHeight;

    LOGD(TAG, "imageSize = %ld, avail = %d", imageSize, pixelCount + pixelCount / 2);

    int64_t y_count = pixelCount;
    int64_t u_count = y_count / 4;
    int64_t v_count = y_count / 4;
    LOGD(TAG, "y_count = %ld, u_count = %ld, v_count = %ld", y_count, u_count, v_count);
    if (y_count + u_count + v_count != imageSize) {
        LOGE(TAG, "size not correct");
        releaseResource();
        return;
    }

    typedef uint8_t DATA_TYPE;
    int GL_INTERNAL_FORMAT = GL_R8;
    int GL_FORMAT = GL_RED;
    int GL_DATA_TYPE = GL_UNSIGNED_BYTE;

    // 申请作为输入的yBuffer和uvBuffer
    // y平面本身已经是独立的，它可以直接给纹理使用
    glGenBuffers(1, &yBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, pixelCount * sizeof(uint8_t), imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // nv21数据先Y，后VU。UV的总长度为总像素量的一半
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pixelCount / 2 * sizeof(uint8_t), imageData + pixelCount * sizeof(uint8_t), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &uBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, u_count, nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, v_count, nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    ComputeProgram computeProgram;
    if (!computeProgram.compile(computeShaderCode)) {
        LOGE(TAG, "compile compute shader failed");
        releaseResource();
        return;
    }

    computeProgram.use();
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, uvBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, uBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vBuffer);

    computeProgram.setInt("image_width", imageWidth);

    // 这里分配的大小跟着色器中设置的local group大小有关。具体可以查看着色器代码中的注释
    glDispatchCompute(imageWidth / 16, imageHeight / 8, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vBuffer);
    uint8_t *ptr = (uint8_t *)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, v_count, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
    for (int i = 0; i < 16; i++) {
        uint8_t splitR = ptr[i];
        LOGD(TAG, "r = %d", splitR);
    }
    for (int i = 1000; i < 1016; i++) {
        uint8_t v1 = imageData[pixelCount + 2 * i];
        uint8_t v2 = ptr[i];
        if (v1 != v2) {
            LOGE(TAG, "v not same, v1 = %d, v2 = %d, i = %d", v1, v2, i);
        }
    }
    //memset(ptr, 0, v_count);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


    // 下个阶段，准备绘制
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glGenTextures(1, &yTex);
    glBindTexture(GL_TEXTURE_2D, yTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth, imageHeight, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, uBuffer);
    glGenTextures(1, &uTex);
    glBindTexture(GL_TEXTURE_2D, uTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth / 2, imageHeight / 2, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vBuffer);
    glGenTextures(1, &vTex);
    glBindTexture(GL_TEXTURE_2D, vTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth / 2, imageHeight / 2, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    float vertexData[20];
    compute_vertex(screenWidth, screenHeight, imageWidth, imageHeight, vertexData);
    create_vertex_objects(VAO, VBO, EBO, vertexData, EBO_indices);

    RenderProgram renderProgram;
    if (!renderProgram.compile(renderVertCode, renderFragCode)) {
        LOGE(TAG, "renderProgram compile failed");
        releaseResource();
        return;
    }
    renderProgram.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, yTex);
    renderProgram.setInt("tex_y", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uTex);
    renderProgram.setInt("tex_u", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vTex);
    renderProgram.setInt("tex_v", 2);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();

    unique_lock<mutex> lock(exitMu);
    exitCond.wait(lock);

    releaseResource();
}

void ComputeShaderTestCase::testFunc_precision(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *computeShaderCode = nullptr;

    GLuint inputBuffer = 0, outputBuffer = 0;

    auto releaseResource = [&]() {
        if (computeShaderCode) {
            free(computeShaderCode);
        }
        if (inputBuffer) {
            glDeleteBuffers(1, &inputBuffer);
        }
        if (outputBuffer) {
            glDeleteBuffers(1, &outputBuffer);
        }
    };

    computeShaderCode = load_shader_code(assetManager, "shaders/precision_test.frag");
    if (computeShaderCode == nullptr) {
        LOGE(TAG, "load shader code failed");
        releaseResource();
        return;
    }

    const int DATA_SIZE = 10;
    int32_t *data = (int32_t *)malloc(DATA_SIZE * sizeof(int32_t));
    for (int i = 0; i < DATA_SIZE; i++) {
        data[i] = 0xAABBCCDD;
    }

    glGenBuffers(1, &inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, DATA_SIZE * sizeof(int32_t), data, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, DATA_SIZE * sizeof(int32_t), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    ComputeProgram computeProgram;
    if (!computeProgram.compile(computeShaderCode)) {
        LOGE(TAG, "compile compute shader failed");
        releaseResource();
        return;
    }

    computeProgram.use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);

    glDispatchCompute(DATA_SIZE, 1, 1);

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    int32_t *ptr = (int32_t *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, DATA_SIZE * sizeof(int32_t), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
    for (int i = 0; i < DATA_SIZE; i++) {
        int32_t num = ptr[i];
        LOGD(TAG, "num = 0x%x", num);
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    releaseResource();

}



void ComputeShaderTestCase::stop() {
    LOGD(TAG, "stop: begin");
    stopFlag = true;
    exitCond.notify_all();
    TestCase::stop();
    LOGD(TAG, "stop: complete");
}
