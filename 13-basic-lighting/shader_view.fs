#version 330 core
out vec4 FragColor;

in vec3 fNorm;
in vec3 fPos;
in vec3 lPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(lPos - fPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(-fPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
