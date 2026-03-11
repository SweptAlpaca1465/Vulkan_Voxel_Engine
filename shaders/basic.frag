#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 lightDirAmbient;
} ubo;

layout(location = 0) in vec3 fragBaseColor;
layout(location = 1) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(ubo.lightDirAmbient.xyz);
    float ambient = clamp(ubo.lightDirAmbient.w, 0.05, 0.95);

    // Half-Lambert for softer voxel lighting falloff.
    float ndotl = dot(normal, lightDir);
    float diffuse = ndotl * 0.5 + 0.5;
    diffuse *= diffuse;

    float lighting = ambient + diffuse * (1.0 - ambient);
    vec3 litColor = fragBaseColor * lighting;

    outColor = vec4(litColor, 1.0);
}
