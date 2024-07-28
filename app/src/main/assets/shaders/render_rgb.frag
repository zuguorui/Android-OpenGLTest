#version 320 es

precision mediump float;

in vec2 TexCoord;

layout (binding = 0) uniform sampler2D rData;
layout (binding = 1) uniform sampler2D gData;
layout (binding = 2) uniform sampler2D bData;

out vec4 fragColor;

void main() {
    float r = texture(rData, TexCoord).r;
    float g = texture(gData, TexCoord).r;
    float b = texture(bData, TexCoord).r;
    fragColor = vec4(r, g, b, 1.0);
}
