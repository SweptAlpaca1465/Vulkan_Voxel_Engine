#pragma once

#include <glm/glm.hpp>

struct UniformBufferObject {
    glm::mat4 model{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 proj{1.0f};
    glm::vec4 lightDirAmbient{0.40f, 0.85f, 0.30f, 0.42f};
};