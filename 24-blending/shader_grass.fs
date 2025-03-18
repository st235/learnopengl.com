#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main() {
    vec4 texColour = texture(texture1, vec2(TexCoords.x, 1.0f-TexCoords.y));
    if (texColour.a < 0.1) {
        discard;
    }
    FragColor = texColour;
}