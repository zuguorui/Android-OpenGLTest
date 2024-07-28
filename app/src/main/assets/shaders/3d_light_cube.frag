#version 330 core

uniform vec3 cubeColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;

out vec4 fragColor;

float specularStrength = 0.5;
float ambientStrength = 0.1;

void main() {
    // 漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * specularStrength * lightColor;
    // 环境光
    vec3 ambient = ambientStrength * lightColor;
    fragColor = vec4(cubeColor * (ambient + diffuse + specular), 1.0f);
}