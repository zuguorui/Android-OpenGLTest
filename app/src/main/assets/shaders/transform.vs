#version 330 core

layout (location = 0) in vec3 mPos;
layout (location = 1) in vec2 mTexCoord;

uniform mat4 transform;

out vec2 TexCoord;

void main() {
    gl_Position = transform * vec4(mPos, 1.0f);
    TexCoord = mTexCoord;
}