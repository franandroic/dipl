#version 450

layout(location = 0) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {

    //NORMAL VISUALIZATION
    outColor = vec4(normalize(fragNormal), 1.0);
}