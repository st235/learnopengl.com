// Specify a horizontal offset via a uniform and move the triangle to the right side of the screen
// in the vertex shader using this offset value.

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

uniform float hor_offset;

void main() {
    vColor = aColor;
    gl_Position = vec4(aPos.x + hor_offset, -aPos.y, aPos.z, 1.0);
}
