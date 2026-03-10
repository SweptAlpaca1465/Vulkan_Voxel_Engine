#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 lightDirAmbient;
} ubo;

layout(location = 0) in vec3 fragWorldPos;
layout(location = 1) in vec3 fragBaseColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 dx = dFdx(fragWorldPos);
    vec3 dy = dFdy(fragWorldPos);
    vec3 normal = normalize(cross(dx, dy));

    if (!gl_FrontFacing) {
        normal = -normal;
    }

    vec3 lightDir = normalize(ubo.lightDirAmbient.xyz);
    float ambient = clamp(ubo.lightDirAmbient.w, 0.05, 0.95);

    float diffuse = max(dot(normal, lightDir), 0.0);
    float lighting = ambient + diffuse * (1.0 - ambient);

    vec3 litColor = fragBaseColor * lighting;
    outColor = vec4(litColor, 1.0);
}
