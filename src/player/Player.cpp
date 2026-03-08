#include "player/Player.hpp"

Player::Player()
    : position(4.0f, 6.0f, 10.0f),
      velocity(0.0f, 0.0f, 0.0f),
      grounded(false),
      halfWidth(0.30f),
      height(1.80f),
      eyeOffset(1.62f),
      selectedBlockType(BlockType::Stone) {
}

const glm::vec3& Player::getPosition() const {
    return position;
}

void Player::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

const glm::vec3& Player::getVelocity() const {
    return velocity;
}

void Player::setVelocity(const glm::vec3& newVelocity) {
    velocity = newVelocity;
}

bool Player::isGrounded() const {
    return grounded;
}

void Player::setGrounded(bool value) {
    grounded = value;
}

glm::vec3 Player::getEyePosition() const {
    return glm::vec3(position.x, position.y + eyeOffset, position.z);
}

float Player::getHalfWidth() const {
    return halfWidth;
}

float Player::getHeight() const {
    return height;
}

float Player::getEyeOffset() const {
    return eyeOffset;
}

BlockType Player::getSelectedBlockType() const {
    return selectedBlockType;
}

void Player::setSelectedBlockType(BlockType type) {
    selectedBlockType = type;
}