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
    struct MovementModifiers {
        float walkSpeed = 0.038f;
        float sprintSpeed = 0.058f;
        float groundAcceleration = 0.22f;
        float airAcceleration = 0.06f;
        float groundFriction = 0.80f;
        float jumpVelocity = 0.065f;
        float gravity = 0.0035f;
    };

    Physics();

    void simulatePlayer(
        Player& player,
        const World& world,
        const glm::vec3& wishMove,
        bool wantsJump,
        bool wantsSprint
    ) const;

    MovementModifiers getMovementModifiers() const;
    void setMovementModifiers(const MovementModifiers& modifiers);

    BlockRaycastHit raycastBlocks(
        const World& world,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float maxDistance
    ) const;

private:
    float gravity;
    float jumpVelocity;
    float walkSpeed;
    float sprintSpeed;
    float groundAcceleration;
    float airAcceleration;
    float groundFriction;

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