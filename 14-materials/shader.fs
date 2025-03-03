#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 fNorm;
in vec3 fPos;

uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main() {
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(light.position - fPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    vec3 viewDir = normalize(viewPos - fPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    FragColor = vec4((ambient + diffuse + specular), 1.0);
}
