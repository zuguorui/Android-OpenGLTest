//
// Created by zu on 2024/7/27.
//

#pragma once

#include <GLES3/gl32.h>
#include <GLES3/gl3platform.h>

/**
 * GL工具函数，这些函数需要再GL环境中调用
 *
 * */

/**
 * 获取VAO、VBO、EBO等顶点数据。
 * vertexData: 顶点数据，是float数组，长度至少为20。
 * indices: EBO顶点顺序数组，长度为至少6，定义一个矩形的顶点绘制顺序。
 * 相关释放由调用方负责
 * */
void create_vertex_objects(GLuint &VAO, GLuint &VBO, GLuint &EBO, float *vertexData, unsigned int *indices);
