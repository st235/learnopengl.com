#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0f / 300.0f;

void main() {
    vec2 offsets[9] = vec2[](
        vec2(-offset, -offset),
        vec2(0, -offset),
        vec2(offset, -offset),
        vec2(-offset, 0),
        vec2(0, 0),
        vec2(offset, 0),
        vec2(-offset, offset),
        vec2(0, offset),
        vec2(offset, offset)
    );

    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }

    vec3 outColor = vec3(0.0f);
    for (int i = 0; i < 9; i++) {
        outColor += kernel[i] * sampleTex[i];
    }

    FragColor = vec4(outColor, 1.0f);
}