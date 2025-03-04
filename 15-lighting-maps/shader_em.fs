#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
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
in vec2 TexCoords;

uniform sampler2D emission;
uniform Material material;
uniform Light light;

uniform vec3 viewPos;

void main() {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(light.position - fPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

    vec3 viewDir = normalize(viewPos - fPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    vec3 inv = vec3(1.0f) - vec3(texture(material.specular, TexCoords));
    vec3 em = inv * vec3(texture(emission, TexCoords));

    FragColor = vec4((em + ambient + diffuse + specular), 1.0);
}
