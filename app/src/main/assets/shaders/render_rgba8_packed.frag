#version 320 es

precision mediump float;

in vec2 TexCoord;

layout (binding = 0) uniform sampler2D rgba_tex;

out vec4 fragColor;

void main() {
    fragColor = vec4(texture(rgba_tex, TexCoord).rgb, 1.0);
}
