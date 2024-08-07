#version 320 es

precision mediump float;

uniform sampler2D y_tex;
uniform sampler2D uv_tex;


in vec2 TexCoord;
out vec4 FragColor;

void main() {
    float y = texture(y_tex, TexCoord).r;
    vec2 uv = texture(uv_tex, TexCoord).rg;
    // nv21是先v后u
    float v = uv.r;
    float u = uv.g;

        float y1 = y;
        float u1 = u;
        float v1 = v;

    y -= 0.0625f;
    u -= 0.5f;
    v -= 0.5f;

    float r = 1.164f * y + 1.793f * v;
    float g = 1.164f * y - 0.213f * u - 0.533f * v;
    float b = 1.164f * y + 2.112f * u;
    FragColor = vec4(r, g, b, 1.0f);
}


