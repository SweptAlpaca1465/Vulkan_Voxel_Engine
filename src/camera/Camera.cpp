#include "camera/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>

Camera::Camera()
    : position(4.0f, 4.0f, 10.0f),
      forward(0.0f, 0.0f, -1.0f),
      up(0.0f, 1.0f, 0.0f),
      worldUp(0.0f, 1.0f, 0.0f),
      yaw(-90.0f),
      pitch(-20.0f) {
    updateVectors();
}

void Camera::moveForward(float amount) {
    position += forward * amount;
}

void Camera::moveRight(float amount) {
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    position += right * amount;
}

void Camera::moveUp(float amount) {
    position += worldUp * amount;
}

void Camera::rotate(float yawOffset, float pitchOffset) {
    yaw += yawOffset;
    pitch += pitchOffset;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    updateVectors();
}

void Camera::updateVectors() {
    glm::vec3 direction;
    direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    direction.y = std::sin(glm::radians(pitch));
    direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));

    forward = glm::normalize(direction);
    up = glm::normalize(glm::cross(glm::normalize(glm::cross(forward, worldUp)), forward));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + forward, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    glm::mat4 proj = glm::perspective(
        glm::radians(70.0f),
        aspectRatio,
        0.1f,
        100.0f
    );
    proj[1][1] *= -1.0f;
    return proj;
}

const glm::vec3& Camera::getPosition() const {
    return position;
}

const glm::vec3& Camera::getForward() const {
    return forward;
}