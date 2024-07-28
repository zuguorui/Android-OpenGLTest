#version 330 core

struct Material {
    // 环境光
    vec3 ambient;
    // 漫反射
    vec3 diffuse;
    // 镜面反射
    vec3 specular;

    // 反光强度
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;

uniform Light light;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform vec3 cubeColor;

out vec4 fragColor;

void main() {
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    fragColor = vec4((ambient + diffuse + specular), 1.0);
    
}

