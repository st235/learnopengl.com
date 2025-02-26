#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

out vec3 ambient;
out vec3 diffuse;
out vec3 specular;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 lightColor;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vec3 fNorm = mat3(transpose(inverse(view))) * mat3(transpose(inverse(model))) * aNorm;
    vec3 fPos = vec3(view * model * vec4(aPos, 1.0));
    vec3 lPos = vec3(view * vec4(lightPos, 1.0));

    float ambientStrength = 0.1;
    ambient = ambientStrength * lightColor;

    vec3 norm = normalize(fNorm);
    vec3 lightDir = normalize(lPos - fPos);

    float diff = max(dot(norm, lightDir), 0.0);
    diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(-fPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    specular = specularStrength * spec * lightColor;
}
