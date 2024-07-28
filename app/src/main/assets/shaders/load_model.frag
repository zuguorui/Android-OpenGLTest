#version 330 core


uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 fragColor;

void main() {
    
    fragColor = texture(texture_diffuse1, TexCoords);
}


