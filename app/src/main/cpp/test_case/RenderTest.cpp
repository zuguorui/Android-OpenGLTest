//
// Created by zu on 2024/7/31.
//

#include "RenderTest.h"
#include "utils.h"
#include "gl_utils.h"
#include "Log.h"
#include <GLES3/gl32.h>
#include <GLES3/gl3platform.h>
#include "RenderProgram.h"
#include "data.h"

#define TAG "RenderTest"

using namespace std;

RenderTest::RenderTest() {

}

RenderTest::~RenderTest() noexcept {

}

GLTask RenderTest::onCreateTask() {
    GLTask task = bind(&RenderTest::testFunc_yuv420p, this, placeholders::_1);
    return task;
}

void RenderTest::testFunc_rgb565(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;
    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 0;
    int imageHeight = 0;
    int pixelCount = imageWidth * imageHeight;

    GLuint rgbBuffer = 0;
    GLuint rgbTex = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
        if (renderVertCode) {
            free(renderVertCode);
        }
        if (renderFragCode) {
            free(renderFragCode);
        }
        if (imageData) {
            free(imageData);
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

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_rgb565.frag");
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

    glGenBuffers(1, &rgbBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, rgbBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, imageSize, imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, rgbBuffer);
    glGenTextures(1, &rgbTex);
    glBindTexture(GL_TEXTURE_2D, rgbTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
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
    renderProgram.setInt("rgb_tex", 0);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();
    releaseResource();
}

void RenderTest::testFunc_nv21(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;
    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 0;
    int imageHeight = 0;
    int pixelCount = imageWidth * imageHeight;

    GLuint yBuffer, uvBuffer = 0;
    GLuint yTex, uvTex = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
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

        if (yTex) {
            glDeleteTextures(1, &yTex);
        }

        if (uvTex) {
            glDeleteTextures(1, &uvTex);
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

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_nv21.frag");
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

    // 将图像数据缓存到buffer中
    glGenBuffers(1, &yBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, pixelCount, imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, uvBuffer);
    // 注意这里的尺寸，uv总数量是像素量的一半
    glBufferData(GL_PIXEL_UNPACK_BUFFER, pixelCount / 2, imageData + pixelCount, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // 创建纹理
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glGenTextures(1, &yTex);
    glBindTexture(GL_TEXTURE_2D, yTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth, imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, uvBuffer);
    glGenTextures(1, &uvTex);
    glBindTexture(GL_TEXTURE_2D, uvTex);
    // 注意这里uv平面的尺寸是图像宽高的一半
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, imageWidth / 2, imageHeight / 2, 0, GL_RG, GL_UNSIGNED_BYTE,
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
    renderProgram.setInt("y_tex", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uvTex);
    renderProgram.setInt("uv_tex", 1);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();
    releaseResource();

}

void RenderTest::testFunc_yuv420p(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;
    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 0;
    int imageHeight = 0;
    int pixelCount = imageWidth * imageHeight;

    GLuint yBuffer, uBuffer = 0, vBuffer = 0;
    GLuint yTex = 0, uTex = 0, vTex = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
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

        if (uBuffer) {
            glDeleteBuffers(1, &uBuffer);
        }

        if (vBuffer) {
            glDeleteBuffers(1, &vBuffer);
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

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_yuv420p.frag");
    if (renderFragCode == nullptr) {
        LOGE(TAG, "load render frag code failed");
        releaseResource();
        return;
    }

    if (!load_asset_file(assetManager, "images/output-1920x1080-yuv420p.yuv", &imageData, &imageSize)) {
        LOGE(TAG, "load image failed");
        releaseResource();
        return;
    }

    imageWidth = 1920;
    imageHeight = 1080;
    pixelCount = imageWidth * imageHeight;

    int64_t yCount = pixelCount;
    int64_t uCount = pixelCount / 4;
    int64_t vCount = uCount;

    // 将图像数据缓存到buffer中
    glGenBuffers(1, &yBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, yCount, imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenBuffers(1, &uBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, uBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, uCount, imageData + yCount, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, vCount, imageData + yCount + uCount, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // 创建纹理
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glGenTextures(1, &yTex);
    glBindTexture(GL_TEXTURE_2D, yTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth, imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, uBuffer);
    glGenTextures(1, &uTex);
    glBindTexture(GL_TEXTURE_2D, uTex);
    // 注意这里uv平面的尺寸是图像宽高的一半
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth / 2, imageHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vBuffer);
    glGenTextures(1, &vTex);
    glBindTexture(GL_TEXTURE_2D, vTex);
    // 注意这里uv平面的尺寸是图像宽高的一半
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth / 2, imageHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE,
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
    renderProgram.setInt("y_tex", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uTex);
    renderProgram.setInt("u_tex", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vTex);
    renderProgram.setInt("v_tex", 2);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();
    releaseResource();
}

void RenderTest::testFunc_yuv422p(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;
    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 0;
    int imageHeight = 0;
    int pixelCount = imageWidth * imageHeight;

    GLuint yBuffer, uBuffer = 0, vBuffer = 0;
    GLuint yTex = 0, uTex = 0, vTex = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
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

        if (uBuffer) {
            glDeleteBuffers(1, &uBuffer);
        }

        if (vBuffer) {
            glDeleteBuffers(1, &vBuffer);
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

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_yuv422p.frag");
    if (renderFragCode == nullptr) {
        LOGE(TAG, "load render frag code failed");
        releaseResource();
        return;
    }

    if (!load_asset_file(assetManager, "images/output-1920x1080-yuv422p.yuv", &imageData, &imageSize)) {
        LOGE(TAG, "load image failed");
        releaseResource();
        return;
    }

    imageWidth = 1920;
    imageHeight = 1080;
    pixelCount = imageWidth * imageHeight;

    // 每2个Y共用一组UV，这里假定是一行相邻的Y，而不是一列相邻的Y。因此UV平面的宽度是图像宽度的一半。
    int64_t yCount = pixelCount;
    int64_t uCount = pixelCount / 2;
    int64_t vCount = uCount;

    // 将图像数据缓存到buffer中
    glGenBuffers(1, &yBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, yCount, imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenBuffers(1, &uBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, uBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, uCount, imageData + yCount, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, vCount, imageData + yCount + uCount, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // 创建纹理
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yBuffer);
    glGenTextures(1, &yTex);
    glBindTexture(GL_TEXTURE_2D, yTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth, imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, uBuffer);
    glGenTextures(1, &uTex);
    glBindTexture(GL_TEXTURE_2D, uTex);
    // 注意这里uv平面的宽度是图像宽度的一半
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth / 2, imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
                 nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, vBuffer);
    glGenTextures(1, &vTex);
    glBindTexture(GL_TEXTURE_2D, vTex);
    // 注意这里uv平面的宽度是图像宽度的一半
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth / 2, imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE,
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
    renderProgram.setInt("y_tex", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uTex);
    renderProgram.setInt("u_tex", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, vTex);
    renderProgram.setInt("v_tex", 2);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();
    releaseResource();
}

void RenderTest::testFunc_yuyv422(EGLWindow &eglWindow) {
    eglWindow.makeCurrent();
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;
    uint8_t *imageData = nullptr;
    int64_t imageSize = 0;
    int imageWidth = 0;
    int imageHeight = 0;
    int pixelCount = imageWidth * imageHeight;

    GLuint yuvBuffer = 0;
    GLuint yuvTex = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
        if (renderVertCode) {
            free(renderVertCode);
        }
        if (renderFragCode) {
            free(renderFragCode);
        }
        if (imageData) {
            free(imageData);
        }
        if (yuvBuffer) {
            glDeleteBuffers(1, &yuvBuffer);
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

    renderVertCode = load_shader_code(assetManager, "shaders/textures.vert");
    if (renderVertCode == nullptr) {
        LOGE(TAG, "load render vertex code failed");
        releaseResource();
        return;
    }

    renderFragCode = load_shader_code(assetManager, "shaders/render_yuyv422.frag");
    if (renderFragCode == nullptr) {
        LOGE(TAG, "load render frag code failed");
        releaseResource();
        return;
    }

    if (!load_asset_file(assetManager, "images/output-1920x1080-yuyv422.yuv", &imageData, &imageSize)) {
        LOGE(TAG, "load image failed");
        releaseResource();
        return;
    }

    imageWidth = 1920;
    imageHeight = 1080;
    pixelCount = imageWidth * imageHeight;

    // 每2个Y共用一组UV，这里假定是一行相邻的Y，而不是一列相邻的Y。因此UV平面的宽度是图像宽度的一半。
    int64_t yCount = pixelCount;
    int64_t uCount = pixelCount / 2;
    int64_t vCount = uCount;

    // 将图像数据缓存到buffer中
    glGenBuffers(1, &yuvBuffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yuvBuffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, imageSize, imageData, GL_STATIC_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);


    // 创建纹理
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, yuvBuffer);
    glGenTextures(1, &yuvTex);
    glBindTexture(GL_TEXTURE_2D, yuvTex);
    // 这里直接把yuyv看作rgba，确保gl可以对每个通道单独采样即可。但是这样会导致Y的采样错误，渲染出的画面模糊。
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imageWidth / 2, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
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
    renderProgram.setInt("yuyv_tex", 0);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    LOGD(TAG, "draw");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    eglWindow.swapBuffer();
    releaseResource();
}

