//
// Created by zu on 2024/7/28.
//

#include "InitTest.h"

#include "utils.h"
#include "gl_utils.h"
#include "Log.h"
#include <GLES3/gl32.h>
#include <GLES3/gl3platform.h>
#include "RenderProgram.h"
#include "data.h"

#define TAG "InitTest"

using namespace std;

InitTest::InitTest() {

}

InitTest::~InitTest() {

}

GLTask InitTest::onCreateTask() {
    GLTask task = bind(&InitTest::taskFunc, this, placeholders::_1);
    return task;
}

void InitTest::taskFunc(EGLWindow &eglWindow) {

    eglWindow.makeCurrent();
    char *renderVertCode = nullptr;
    char *renderFragCode = nullptr;

    int imageWidth = 1536;
    int imageHeight = 864;
    int pixelCount = imageWidth * imageHeight;

    GLuint rTex = 0, gTex = 0, bTex = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;

    auto releaseResource = [&]() {
        if (renderVertCode) {
            free(renderVertCode);
        }
        if (renderFragCode) {
            free(renderFragCode);
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

    typedef uint8_t DATA_FORMAT;
    int GL_DATA_FORMAT = GL_UNSIGNED_BYTE;

    DATA_FORMAT *tempBuffer1 = (DATA_FORMAT *) malloc(pixelCount * sizeof(DATA_FORMAT));
    for (int i = 0; i < pixelCount; i++) {
        tempBuffer1[i] = INT8_MAX;
    }
//    memset(tempBuffer1, 1, pixelCount * sizeof(DATA_FORMAT));

    DATA_FORMAT *tempBuffer0 = (DATA_FORMAT *) malloc(pixelCount * sizeof(DATA_FORMAT));
    memset(tempBuffer0, 0, pixelCount * sizeof(DATA_FORMAT));

    glGenTextures(1, &rTex);
    glBindTexture(GL_TEXTURE_2D, rTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth, imageHeight, 0, GL_RED, GL_DATA_FORMAT, tempBuffer1);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RED, GL_DATA_FORMAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &gTex);
    glBindTexture(GL_TEXTURE_2D, gTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth, imageHeight, 0, GL_RED, GL_DATA_FORMAT, tempBuffer1);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RED, GL_DATA_FORMAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &bTex);
    glBindTexture(GL_TEXTURE_2D, bTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, imageWidth, imageHeight, 0, GL_RED, GL_DATA_FORMAT, tempBuffer0);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageWidth, imageHeight, GL_RED, GL_DATA_FORMAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    float vertexData[20];
    compute_vertex(screenWidth, screenHeight, imageWidth, imageHeight, vertexData);
    create_vertex_objects(VAO, VBO, EBO, vertexData, EBO_indices);

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

    releaseResource();
}

