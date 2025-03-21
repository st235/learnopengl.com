#version 330 core
out vec4 FragColor;

in vec2 textureCoords;

uniform sampler2D inTexture1;
uniform sampler2D inTexture2;

void main() {
    FragColor = mix(texture(inTexture1, textureCoords),
        texture(inTexture2, textureCoords), 0.2);
}
