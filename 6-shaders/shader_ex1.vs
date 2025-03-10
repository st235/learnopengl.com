// Adjust the vertex shader so that the triangle is upside down.

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    vColor = aColor;
    gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);
}
