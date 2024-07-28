#version 330 core

in vec2 TexCoord;

uniform sampler2D tex_rgb;

out vec4 fragColor;

void main() {
    fragColor = texture(tex_rgb, TexCoord);
}