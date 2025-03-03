#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

out vec3 fNorm;
out vec3 fPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fNorm = mat3(transpose(inverse(model))) * aNorm;
    fPos = vec3(model * vec4(aPos, 1.0));
}
