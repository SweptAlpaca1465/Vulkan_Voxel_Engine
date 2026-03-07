#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera();

    void moveForward(float amount);
    void moveRight(float amount);
    void moveUp(float amount);

    void rotate(float yawOffset, float pitchOffset);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

private:
    void updateVectors();

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
};