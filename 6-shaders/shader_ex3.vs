// Output the vertex position to the fragment shader using the out keyword and set the frag-
// ment’s color equal to this vertex position (see how even the vertex position values are
// interpolated across the triangle). Once you managed to do this; try to answer the following
// question: why is the bottom-left side of our triangle black?

#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 vColor;

void main() {
    vec4 finalPosition = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vColor = finalPosition.xyz;
    gl_Position = finalPosition;
}
