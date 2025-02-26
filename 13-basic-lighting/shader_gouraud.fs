#version 330 core
out vec4 FragColor;

in vec3 ambient;
in vec3 diffuse;
in vec3 specular;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
