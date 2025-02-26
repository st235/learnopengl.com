#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

out vec3 fNorm;
out vec3 fPos;
out vec3 lPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fNorm = mat3(transpose(inverse(view))) * mat3(transpose(inverse(model))) * aNorm;
    fPos = vec3(view * model * vec4(aPos, 1.0));
    lPos = vec3(view * vec4(lightPos, 1.0));
}
