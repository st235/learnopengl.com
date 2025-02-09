#version 330 core
out vec4 FragColor;

in vec3 fColor;
in vec2 textureCoords;

uniform sampler2D inTexture1;
uniform sampler2D inTexture2;
uniform float mixRate;

void main() {
    FragColor = mix(texture(inTexture1, textureCoords),
        texture(inTexture2, vec2(-textureCoords.x, -textureCoords.y)), mixRate) * vec4(fColor, 1.0f);
}
