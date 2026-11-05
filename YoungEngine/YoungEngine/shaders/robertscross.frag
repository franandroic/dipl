#version 450

layout(set = 1, binding = 0) uniform sampler2D mask;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

const vec3 OUTLINE_COLOR = vec3(0.1, 0.8, 1.0);

const float STEP = 2.0;

void main() {

    vec2 texel = STEP / vec2(textureSize(mask, 0));

    float topLeft     = texture(mask, uv).r;
    float topRight    = texture(mask, uv + vec2(texel.x, 0.0)).r;
    float bottomLeft  = texture(mask, uv + vec2(0.0, texel.y)).r;
    float bottomRight = texture(mask, uv + texel).r;

    float gx = topLeft - bottomRight;
    float gy = topRight - bottomLeft;

    float edge = clamp(sqrt(gx * gx + gy * gy), 0.0, 1.0);

    outColor = vec4(OUTLINE_COLOR, edge);
}
