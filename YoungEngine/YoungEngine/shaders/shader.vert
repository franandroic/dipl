#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

vec3 lightPos = vec3(2.0, -2.0, 2.0);
vec3 cameraPos = vec3(0.0, -2.0, 0.0);

void main() {
    vec4 vPos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * vPos;
    fragColor = inColor;
    fragNormal = inNormal;

    float normalLightAngle = max(0.0, dot(normalize(lightPos - vec3(vPos)), inNormal));
    float normalCameraAngle = max(0.0, dot(normalize(cameraPos - vec3(vPos)), inNormal));
    fragTexCoord = vec2(normalLightAngle, normalCameraAngle);
}