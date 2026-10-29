#version 450

//Shader pair of the axes: the lines are drawn in the colors of their vertices, without lighting or textures.
//Only sets 0 (the camera) and the push constants are used, matching the pipeline Axes describes.

layout(set = 0, binding = 0) uniform CameraUniformBuffer {
    mat4 view;
    mat4 proj;
    vec3 position;
} camera;

layout(push_constant) uniform ModelPushConstant {
    mat4 model;
} pc;

//The pipeline provides every Vertex attribute, but the axes only need these two
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {

    gl_Position = camera.proj * camera.view * pc.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
