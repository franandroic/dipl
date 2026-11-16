#version 450

layout(set = 0, binding = 0) uniform CameraBlock {
    mat4 view;
    mat4 proj;
    vec3 position;
} camera;

layout(set = 2, binding = 0) uniform LightBlock {
    float intensity;
    vec3 position;
} light;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 P;
layout(location = 2) in vec3 N;

layout(location = 0) out vec4 outColor;

const float ks = 0.1;
const float kd = 0.5;
const float ka = 0.2;
const float alpha = 50;
const float ia = 0.8;

vec3 debugColor = vec3(0.75, 0.75, 0.75);

void main() {

    vec3 nN = normalize(N);
    vec3 L = normalize(light.position - P);
    vec3 R = reflect(-L, nN);
    vec3 V = normalize(camera.position - P);

    float phongFactor = ka * ia + (kd * max(dot(L, nN), 0.0) * light.intensity + ks * pow(max(dot(R, V), 0.0), alpha) * light.intensity);
    outColor = vec4(debugColor * phongFactor, 1.0);
}