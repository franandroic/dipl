#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {

    //TEXTURE SAMPLING
    outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, 1.0);

    //NORMAL VISUALIZATION
    /*
    float normalSum = fragNormal.x + fragNormal.y + fragNormal.z;
    vec3 normalColor;
    if (normalSum >= 0) normalColor = vec3(1.0, 0.0, 0.0);
    else normalColor = vec3(0.0, 0.0, 1.0);
    outColor = vec4(normalColor, 1.0);
    */
}