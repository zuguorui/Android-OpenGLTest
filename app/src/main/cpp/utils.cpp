//
// Created by zu on 2024/7/26.
//

#include "utils.h"

#include <stdlib.h>
#include <iostream>
#include "Log.h"

#define TAG "utils"

using namespace std;

bool load_asset_file(AAssetManager *assetManager, const char *path, uint8_t **data, int64_t *size) {
    AAsset *asset = AAssetManager_open(assetManager, path, AASSET_MODE_BUFFER);
    if (asset == nullptr) {
        LOGE(TAG, "failed to open asset file: %s", path);
        return false;
    }
    *size = AAsset_getLength64(asset);
    *data = (uint8_t *) malloc(*size);
    memset(*data, 0, *size);
    int64_t readCount = AAsset_read(asset, *data, *size);
    if (readCount != *size) {
        LOGE(TAG, "failed to read from asset: %s, expect size %ld, actual size %d", path, *size, readCount);
        free(*data);
        *data = nullptr;
        *size = 0;
        return false;
    }
    AAsset_close(asset);
    return true;
}

char* load_shader_code(AAssetManager *assetManager, const char *path) {
    char *temp = nullptr;
    int64_t length = 0;
    if (!load_asset_file(assetManager, path, (uint8_t **)&temp, &length)) {
        LOGE(TAG, "load shader code failed: %s", path);
        return nullptr;
    }
    char *code = (char *)malloc(length + 1);
    memcpy(code, temp, length);
    free(temp);
    code[length] = '\0';
    //LOGD(TAG, "shaderCode: \n%s", code);
    return code;
}

bool compute_vertex(int screenWidth, int screenHeight, int imageWidth, int imageHeight, float *result) {
    if (screenWidth == 0 || screenHeight == 0 || imageWidth == 0 || imageHeight == 0 || result == nullptr) {
        return false;
    }

    float screenLeft, screenRight, screenTop, screenBottom;
    float frameLeft, frameRight, frameTop, frameBottom;

    frameLeft = 0;
    frameRight = 1;
    frameTop = 1;
    frameBottom = 0;

    float frameW2H = imageWidth * 1.0f / imageHeight;
    float screenW2H = screenWidth * 1.0f / screenHeight;

    if (frameW2H >= screenW2H) {
        int scaledScreenHeight = (int)(screenWidth * imageHeight * 1.0f / imageWidth);
        screenLeft = -1;
        screenRight = 1;
        screenTop = scaledScreenHeight * 1.0f / screenHeight;
        screenBottom = -screenTop;
    } else {
        int scaledScreenWidth = (int)(screenHeight * imageWidth * 1.0f / imageHeight);
        screenRight = scaledScreenWidth * 1.0f / screenWidth;
        screenLeft = -screenRight;

        screenTop = 1;
        screenBottom = -1;
    }

    float tmpVert[20] = {
            screenLeft, screenBottom, 0, frameLeft, frameTop,
            screenRight, screenBottom, 0, frameRight, frameTop,
            screenRight, screenTop, 0, frameRight, frameBottom,
            screenLeft, screenTop, 0, frameLeft, frameBottom,
    };

    memcpy(result, tmpVert, 20 * sizeof(float));

    return true;
}

