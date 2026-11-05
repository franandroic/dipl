#version 450

layout(set = 0, binding = 0) uniform CameraUniformBuffer {
    mat4 view;
    mat4 proj;
    vec3 position;
} camera;

layout(push_constant) uniform ModelPushConstant {
    mat4 model;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec3 inNormal;

const float OUTLINE_WIDTH = 0.04;

void main() {

    vec3 pushed = inPosition + normalize(inNormal) * OUTLINE_WIDTH;
    gl_Position = camera.proj * camera.view * pc.model * vec4(pushed, 1.0);
}
