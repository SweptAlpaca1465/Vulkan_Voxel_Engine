#pragma once

#include "player/Player.hpp"
#include "world/World.hpp"

#include <glm/glm.hpp>

struct PhysicsAABB {
    glm::vec3 min{0.0f};
    glm::vec3 max{0.0f};
};

struct BlockRaycastHit {
    bool hit = false;
    glm::ivec3 block{0};
    glm::ivec3 placeBlock{0};
    glm::ivec3 hitNormal{0};
    BlockType blockType = BlockType::Air;
};

class Physics {
public:
    Physics();

    void simulatePlayer(
        Player& player,
        const World& world,
        const glm::vec3& wishMove,
        bool wantsJump
    ) const;

    BlockRaycastHit raycastBlocks(
        const World& world,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float maxDistance
    ) const;

private:
    float gravity;
    float jumpVelocity;
    float horizontalMoveSpeed;
    float airControlFactor;

    PhysicsAABB getPlayerAABBAt(const Player& player, const glm::vec3& position) const;
    bool isSolidBlockAt(const World& world, int x, int y, int z) const;
    bool intersectsSolidBlock(const World& world, const PhysicsAABB& box) const;

    void movePlayerAxis(
        Player& player,
        const World& world,
        glm::vec3& position,
        int axis,
        float amount
    ) const;
};