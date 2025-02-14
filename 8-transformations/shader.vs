#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

out vec3 fColor;
out vec2 textureCoords;

uniform mat4 inMat;

void main() {
    gl_Position = inMat * vec4(aPos, 1.0);
    fColor = aColor;
    textureCoords = aTex;
}
