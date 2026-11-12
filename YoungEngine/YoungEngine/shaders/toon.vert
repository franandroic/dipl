#version 450

layout(set = 0, binding = 0) uniform CameraUniformBuffer {
    mat4 view;
    mat4 proj;
    vec3 position;
} camera;

layout(set = 2, binding = 0) uniform LightBlock {
    float intensity;
    vec3 position;
} light;

layout(push_constant) uniform ModelPushConstant {
    mat4 model;
} pc;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

void main() {

    vec4 vPos = pc.model * vec4(inPosition, 1.0);
    gl_Position = camera.proj * camera.view * vPos;
    
    fragColor = inColor;
    
    vec3 worldNormal = normalize(mat3(pc.model) * inNormal);
    fragNormal = worldNormal;

    float normalLightAngle = max(0.0, dot(normalize(light.position - vec3(vPos)), worldNormal));
    float normalCameraAngle = max(0.0, dot(normalize(camera.position - vec3(vPos)), worldNormal));
    fragTexCoord = vec2(normalLightAngle, normalCameraAngle);
}