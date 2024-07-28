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

}

GLTask ComputeShaderTestCase::onCreateTask() {
    GLTask task = bind(&ComputeShaderTestCase::taskFunc, this, placeholders::_1);
    return task;
}

void ComputeShaderTestCase::taskFunc(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *computeShaderCode = nullptr;
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;

    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 1536;
    int imageHeight = 864;
    int pixelCount = imageWidth * imageHeight;

    GLuint rawBuffer = 0, rBuffer = 0, gBuffer = 0, bBuffer = 0;
    GLuint rawTex = 0, rTex = 0, gTex = 0, bTex = 0;

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
        if (rBuffer) {
            glDeleteBuffers(1, &rBuffer);
        }
        if (gBuffer) {
            glDeleteBuffers(1, &gBuffer);
        }
        if (bBuffer) {
            glDeleteBuffers(1, &bBuffer);
        }

        if (rawTex) {
            glDeleteTextures(1, &rawTex);
        }
        if (rTex) {
            glDeleteTextures(1, &rTex);
        }
        if (gTex) {
            glDeleteTextures(1, &gTex);
        }
        if (bTex) {
            glDeleteTextures(1, &bTex);
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


    computeShaderCode = load_shader_code(assetManager, "shaders/split_rgb565.frag");
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

    renderFragCode = load_shader_code(assetManager, "shaders/render_rgb.frag");
    if (renderFragCode == nullptr) {
        LOGE(TAG, "load render frag code failed");
        releaseResource();
        return;
    }


    if (!load_asset_file(assetManager, "images/out-1536*864-rgb565le.rgb", &imageData, &imageSize)) {
        LOGE(TAG, "load image failed");
        releaseResource();
        return;
    }

    imageWidth = 1536;
    imageHeight = 864;
    pixelCount = imageWidth * imageHeight;

//    if (imageWidth * imageHeight != imageSize) {
//        LOGE(TAG, "image size wrong, width = %d, height = %d, imageSize = %ld", imageWidth, imageHeight, imageSize);
//        releaseResource();
//        return;
//    }

    typedef uint16_t DATA_TYPE;
    int GL_INTERNAL_FORMAT = GL_R8;
    int GL_FORMAT = GL_RED;
    int GL_DATA_TYPE = GL_UNSIGNED_BYTE;

    glGenBuffers(1, &rawBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, imageSize, imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // 申请三通道的buffer，用来存储分离后的通道数据。分离之后的数据都是uint8_t的

    glGenBuffers(1, &rBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pixelCount * sizeof(DATA_TYPE), nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &gBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pixelCount * sizeof(DATA_TYPE), nullptr, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &bBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bBuffer);
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
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, gBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, gBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, bBuffer);

    computeProgram.setUInt("width", imageWidth);
    computeProgram.setUInt("height", imageHeight);

    glDispatchCompute(1, 1, 1);

    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);


    // 下个阶段，准备绘制

    DATA_TYPE *tempBuffer1 = (DATA_TYPE *) malloc(pixelCount * sizeof(DATA_TYPE));
    for (int i = 0; i < pixelCount; i++) {
        tempBuffer1[i] = INT8_MAX;
    }

    DATA_TYPE *tempBuffer0 = (DATA_TYPE *) malloc(pixelCount * sizeof(DATA_TYPE));
    memset(tempBuffer0, 0, pixelCount * sizeof(DATA_TYPE));

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, rBuffer);
    DATA_TYPE *redPtr = (DATA_TYPE *)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, pixelCount * sizeof(DATA_TYPE), GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
    LOGD(TAG, "redPtr = %ld", redPtr);

    for (int i = 0; i < 16; i++) {
        DATA_TYPE splitR = redPtr[i];
        LOGD(TAG, "r  = 0x%x", splitR);
    }
    //memset(redPtr, 0x0F, pixelCount * sizeof(DATA_TYPE));
    //memcpy(redPtr, tempBuffer1, pixelCount * sizeof(DATA_TYPE));
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);


    // 分别构造rgb三个通道的texture
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, rBuffer);
    glGenTextures(1, &rTex);
    glBindTexture(GL_TEXTURE_2D, rTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth, imageHeight, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RED, GL_DATA_TYPE, nullptr);
    //glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, gBuffer);
    glGenTextures(1, &gTex);
    glBindTexture(GL_TEXTURE_2D, gTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth, imageHeight, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RED, GL_DATA_TYPE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bBuffer);
    glGenTextures(1, &bTex);
    glBindTexture(GL_TEXTURE_2D, bTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTERNAL_FORMAT, imageWidth, imageHeight, 0, GL_FORMAT, GL_DATA_TYPE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RED, GL_DATA_TYPE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    glBindTexture(GL_TEXTURE_2D, rTex);
    renderProgram.setInt("rData", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTex);
    renderProgram.setInt("gData", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, bTex);
    renderProgram.setInt("bData", 2);

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

void ComputeShaderTestCase::stop() {
    stopFlag = true;
    exitCond.notify_all();
    TestCase::stop();
}
