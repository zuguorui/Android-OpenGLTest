#version 320 es

precision mediump float;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

in vec2 TexCoord;
out vec4 FragColor;

void main() {
    float y = texture(tex_y, TexCoord).r;
    float u = texture(tex_u, TexCoord).r;
    float v = texture(tex_v, TexCoord).r;

//    float y1 = y;
//    float u1 = u;
//    float v1 = v;

    y -= 0.0625f;
    u -= 0.5f;
    v -= 0.5f;

    float r = 1.164f * y + 1.793f * v;
    float g = 1.164f * y - 0.213f * u - 0.533f * v;
    float b = 1.164f * y + 2.112f * u;
    FragColor = vec4(r, g, b, 1.0f);
}


