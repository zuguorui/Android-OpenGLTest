//
// Created by zu on 2024/7/26.
//

#pragma once

#include <iostream>
#include <stdlib.h>
#include <android/asset_manager.h>

bool load_asset_file(AAssetManager *assetManager, const char *path, uint8_t **data, int64_t *size);

char* load_shader_code(AAssetManager *assetManager, const char *path);

bool compute_vertex(int screenWidth, int screenHeight, int imageWidth, int imageHeight, float *result);

