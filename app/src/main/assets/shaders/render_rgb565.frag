#version 320 es

precision mediump float;

in vec2 TexCoord;

layout (binding = 0) uniform sampler2D rgb_tex;

out vec4 fragColor;

void main() {
    vec3 rgb = texture(rgb_tex, TexCoord).rgb;
    fragColor = vec4(rgb.r, rgb.g, rgb.b, 1.0);
}
