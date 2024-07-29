#version 320 es

precision mediump float;

in vec2 TexCoord;

// 接受的纹理格式为yuvxyuvxyuvx这样打包在一起的。
uniform sampler2D yuvx_tex;

out vec4 fragColor;

void main() {
    // bt709
    vec4 yuvx = texture(yuvx_tex, TexCoord);
    float y = yuvx.r - 0.0625f;
    float u = yuvx.g - 0.5f;
    float v = yuvx.b - 0.5f;
    float r = 1.164f * y + 1.793f * v;
    float g = 1.164f * y - 0.213f * u - 0.533f * v;
    float b = 1.164f * y + 2.112f * u;
    fragColor = vec4(r, g, b, 1.0);
}