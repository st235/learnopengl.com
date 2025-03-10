#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight {
    // Facing outwards from the light source.
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 fNorm;
in vec3 fPos;
in vec2 TexCoords;

uniform Material material;
uniform DirectionalLight light;

uniform vec3 viewPos;

void main() {
    // As the light direction is facing outwards,
    // but we use direction towards the light source
    // during calculation phase.
    vec3 lightDir = -normalize(light.direction);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 norm = normalize(fNorm);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

    vec3 viewDir = normalize(viewPos - fPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    FragColor = vec4((ambient + diffuse + specular), 1.0);
}
