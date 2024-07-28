#version 330 core

uniform vec3 cubeColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(cubeColor, 1.0f);
}