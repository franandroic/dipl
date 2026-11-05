#version 450

layout(location = 0) out vec4 outColor;

const vec3 OUTLINE_COLOR = vec3(1.0, 0.6, 0.1);

void main() {

    outColor = vec4(OUTLINE_COLOR, 1.0);
}
