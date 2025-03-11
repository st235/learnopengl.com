#version 330 core
out vec4 FragColor;

uniform vec3 diffuseColour;

void main() {
    FragColor = vec4(diffuseColour, 1.0);
}