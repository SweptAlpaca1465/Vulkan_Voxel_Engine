#include "world/Physics.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

Physics::Physics()
    : gravity(0.0035f),
      jumpVelocity(0.065f),
      walkSpeed(0.038f),
      sprintSpeed(0.058f),
      groundAcceleration(0.22f),
      airAcceleration(0.06f),
      groundFriction(0.80f) {
}


Physics::MovementModifiers Physics::getMovementModifiers() const {
    MovementModifiers modifiers{};
    modifiers.walkSpeed = walkSpeed;
    modifiers.sprintSpeed = sprintSpeed;
    modifiers.groundAcceleration = groundAcceleration;
    modifiers.airAcceleration = airAcceleration;
    modifiers.groundFriction = groundFriction;
    modifiers.jumpVelocity = jumpVelocity;
    modifiers.gravity = gravity;
    return modifiers;
}

void Physics::setMovementModifiers(const MovementModifiers& modifiers) {
    walkSpeed = std::clamp(modifiers.walkSpeed, 0.005f, 0.20f);
    sprintSpeed = std::clamp(modifiers.sprintSpeed, walkSpeed, 0.30f);
    groundAcceleration = std::clamp(modifiers.groundAcceleration, 0.01f, 1.0f);
    airAcceleration = std::clamp(modifiers.airAcceleration, 0.005f, 1.0f);
    groundFriction = std::clamp(modifiers.groundFriction, 0.10f, 0.99f);
    jumpVelocity = std::clamp(modifiers.jumpVelocity, 0.01f, 0.20f);
    gravity = std::clamp(modifiers.gravity, 0.0005f, 0.03f);
}

void Physics::simulatePlayer(
    Player& player,
    const World& world,
    const glm::vec3& wishMove,
    bool wantsJump,
    bool wantsSprint
) const {
    glm::vec3 velocity = player.getVelocity();
    glm::vec3 position = player.getPosition();

    const bool wasGrounded = player.isGrounded();
    const float targetSpeed = wantsSprint ? sprintSpeed : walkSpeed;

    glm::vec2 horizontalVelocity(velocity.x, velocity.z);
    glm::vec2 desiredVelocity(wishMove.x, wishMove.z);

    if (glm::length(desiredVelocity) > 0.0001f) {
        desiredVelocity = glm::normalize(desiredVelocity) * targetSpeed;
        const float accel = wasGrounded ? groundAcceleration : airAcceleration;
        horizontalVelocity += (desiredVelocity - horizontalVelocity) * accel;
    } else if (wasGrounded) {
        horizontalVelocity *= groundFriction;
        if (glm::length(horizontalVelocity) < 0.0001f) {
            horizontalVelocity = glm::vec2(0.0f);
        }
    }

    velocity.x = horizontalVelocity.x;
    velocity.z = horizontalVelocity.y;

    if (wantsJump && wasGrounded) {
        velocity.y = jumpVelocity;
        player.setGrounded(false);
    }

    player.setGrounded(false);
    velocity.y -= gravity;

    player.setVelocity(velocity);

    movePlayerAxis(player, world, position, 0, velocity.x);
    movePlayerAxis(player, world, position, 1, velocity.y);
    movePlayerAxis(player, world, position, 2, velocity.z);

    player.setPosition(position);
}

BlockRaycastHit Physics::raycastBlocks(
    const World& world,
    const glm::vec3& origin,
    const glm::vec3& directionIn,
    float maxDistance
) const {
    BlockRaycastHit result;

    if (glm::length(directionIn) < 0.000001f) {
        return result;
    }

    const glm::vec3 direction = glm::normalize(directionIn);
    constexpr float epsilon = 0.0001f;

    glm::ivec3 cell(
        static_cast<int>(std::floor(origin.x)),
        static_cast<int>(std::floor(origin.y)),
        static_cast<int>(std::floor(origin.z))
    );

    const int stepX = (direction.x > 0.0f) ? 1 : (direction.x < 0.0f ? -1 : 0);
    const int stepY = (direction.y > 0.0f) ? 1 : (direction.y < 0.0f ? -1 : 0);
    const int stepZ = (direction.z > 0.0f) ? 1 : (direction.z < 0.0f ? -1 : 0);

    const float tDeltaX = (stepX != 0)
        ? std::abs(1.0f / direction.x)
        : std::numeric_limits<float>::infinity();

    const float tDeltaY = (stepY != 0)
        ? std::abs(1.0f / direction.y)
        : std::numeric_limits<float>::infinity();

    const float tDeltaZ = (stepZ != 0)
        ? std::abs(1.0f / direction.z)
        : std::numeric_limits<float>::infinity();

    float tMaxX = std::numeric_limits<float>::infinity();
    float tMaxY = std::numeric_limits<float>::infinity();
    float tMaxZ = std::numeric_limits<float>::infinity();

    if (stepX > 0) {
        tMaxX = (static_cast<float>(cell.x + 1) - origin.x) / direction.x;
    } else if (stepX < 0) {
        tMaxX = (origin.x - static_cast<float>(cell.x)) / -direction.x;
    }

    if (stepY > 0) {
        tMaxY = (static_cast<float>(cell.y + 1) - origin.y) / direction.y;
    } else if (stepY < 0) {
        tMaxY = (origin.y - static_cast<float>(cell.y)) / -direction.y;
    }

    if (stepZ > 0) {
        tMaxZ = (static_cast<float>(cell.z + 1) - origin.z) / direction.z;
    } else if (stepZ < 0) {
        tMaxZ = (origin.z - static_cast<float>(cell.z)) / -direction.z;
    }

    glm::ivec3 hitNormal(0);

    for (;;) {
        BlockType block = BlockType::Air;
        world.getBlockGlobal(cell.x, cell.y, cell.z, block);

        if (block != BlockType::Air) {
            result.hit = true;
            result.block = cell;
            result.hitNormal = hitNormal;
            result.placeBlock = cell + hitNormal;
            result.blockType = block;
            return result;
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                if (tMaxX > maxDistance) {
                    break;
                }

                cell.x += stepX;
                hitNormal = glm::ivec3(-stepX, 0, 0);
                tMaxX += tDeltaX;
            } else {
                if (tMaxZ > maxDistance) {
                    break;
                }

                cell.z += stepZ;
                hitNormal = glm::ivec3(0, 0, -stepZ);
                tMaxZ += tDeltaZ;
            }
        } else {
            if (tMaxY < tMaxZ) {
                if (tMaxY > maxDistance) {
                    break;
                }

                cell.y += stepY;
                hitNormal = glm::ivec3(0, -stepY, 0);
                tMaxY += tDeltaY;
            } else {
                if (tMaxZ > maxDistance) {
                    break;
                }

                cell.z += stepZ;
                hitNormal = glm::ivec3(0, 0, -stepZ);
                tMaxZ += tDeltaZ;
            }
        }

        if (std::min({tMaxX, tMaxY, tMaxZ}) > maxDistance + epsilon) {
            break;
        }
    }

    return result;
}

PhysicsAABB Physics::getPlayerAABBAt(const Player& player, const glm::vec3& position) const {
    return {
        glm::vec3(
            position.x - player.getHalfWidth(),
            position.y,
            position.z - player.getHalfWidth()
        ),
        glm::vec3(
            position.x + player.getHalfWidth(),
            position.y + player.getHeight(),
            position.z + player.getHalfWidth()
        )
    };
}

bool Physics::isSolidBlockAt(const World& world, int x, int y, int z) const {
    BlockType block = BlockType::Air;
    world.getBlockGlobal(x, y, z, block);
    return block != BlockType::Air;
}

bool Physics::intersectsSolidBlock(const World& world, const PhysicsAABB& box) const {
    const int minX = static_cast<int>(std::floor(box.min.x));
    const int minY = static_cast<int>(std::floor(box.min.y));
    const int minZ = static_cast<int>(std::floor(box.min.z));

    const int maxX = static_cast<int>(std::floor(box.max.x - 0.0001f));
    const int maxY = static_cast<int>(std::floor(box.max.y - 0.0001f));
    const int maxZ = static_cast<int>(std::floor(box.max.z - 0.0001f));

    for (int z = minZ; z <= maxZ; ++z) {
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                if (isSolidBlockAt(world, x, y, z)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Physics::movePlayerAxis(
    Player& player,
    const World& world,
    glm::vec3& position,
    int axis,
    float amount
) const {
    if (std::abs(amount) < 0.000001f) {
        return;
    }

    glm::vec3 velocity = player.getVelocity();

    constexpr int steps = 8;
    const float stepAmount = amount / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i) {
        glm::vec3 trial = position;

        if (axis == 0) {
            trial.x += stepAmount;
        } else if (axis == 1) {
            trial.y += stepAmount;
        } else {
            trial.z += stepAmount;
        }

        const PhysicsAABB box = getPlayerAABBAt(player, trial);

        if (!intersectsSolidBlock(world, box)) {
            position = trial;
        } else {
            if (axis == 1) {
                if (stepAmount < 0.0f) {
                    player.setGrounded(true);
                }
                velocity.y = 0.0f;
                player.setVelocity(velocity);
            } else if (axis == 0) {
                velocity.x = 0.0f;
                player.setVelocity(velocity);
            } else if (axis == 2) {
                velocity.z = 0.0f;
                player.setVelocity(velocity);
            }

            return;
        }
    }
}