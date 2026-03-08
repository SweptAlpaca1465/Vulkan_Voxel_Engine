#pragma once

#include "world/Block.hpp"
#include <glm/glm.hpp>

class Player {
public:
    Player();

    const glm::vec3& getPosition() const;
    void setPosition(const glm::vec3& newPosition);

    const glm::vec3& getVelocity() const;
    void setVelocity(const glm::vec3& newVelocity);

    bool isGrounded() const;
    void setGrounded(bool grounded);

    glm::vec3 getEyePosition() const;

    float getHalfWidth() const;
    float getHeight() const;
    float getEyeOffset() const;

    BlockType getSelectedBlockType() const;
    void setSelectedBlockType(BlockType type);

private:
    glm::vec3 position;
    glm::vec3 velocity;
    bool grounded;

    float halfWidth;
    float height;
    float eyeOffset;

    BlockType selectedBlockType;
};