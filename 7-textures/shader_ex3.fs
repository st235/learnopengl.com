#version 330 core
out vec4 FragColor;

in vec3 fColor;
in vec2 textureCoords;

uniform sampler2D inTexture1;

void main() {
    FragColor = texture(inTexture1, textureCoords) * vec4(fColor, 1.0f);
}
