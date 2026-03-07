#pragma once

#include <glm/glm.hpp>

struct UniformBufferObject {
    glm::mat4 model{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 proj{1.0f};
};