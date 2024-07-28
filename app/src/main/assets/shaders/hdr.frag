#version 330 core

uniform sampler2D tex_r;
uniform sampler2D tex_g;
uniform sampler2D tex_b;

in vec2 TexCoord;

out vec4 FragColor;

void main() {
    float r = texture(tex_r, TexCoord).r;
    float g = texture(tex_g, TexCoord).r;
    float b = texture(tex_b, TexCoord).r;

    // if (r > 1) {
    //     r = 1;
    // }
    // if (g > 1) {
    //     g = 1;
    // }
    // if (b > 1) {
    //     b = 1;
    // }

    FragColor = vec4(r, g, b, 1.0f);
}

