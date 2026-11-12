#version 450

layout(set = 0, binding = 0) uniform CameraBlock {
    mat4 view;
    mat4 proj;
    vec3 position;
} camera;

layout(push_constant) uniform ModelPushConstant {
    mat4 model;
} pc;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 P;
layout(location = 2) out vec3 N;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

void main() {

    vec4 vPos = pc.model * vec4(inPosition, 1.0);
    gl_Position = camera.proj * camera.view * vPos;

    fragColor = inColor;
    P = vPos.xyz;
    N = normalize(mat3(pc.model) * inNormal);
}