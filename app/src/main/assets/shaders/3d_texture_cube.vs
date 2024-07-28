#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// 物体在世界坐标系中的摆放位置
uniform mat4 model;
// 将世界坐标系转到视角坐标中，这一步可以将物体转化为视角看到的效果
uniform mat4 view;
// 投影坐标，这一步根据各点到观察位置的距离体现出近大远小
uniform mat4 projection;

out vec2 TexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    TexCoord = aTexCoord;
}