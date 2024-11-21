#include "camera.h"

using namespace glm;

Camera::Camera(glm::vec3 position, glm::vec3 forward) {

    this->position = position;
    this->forward = forward;

    movement_speed = 40.0f;
    sensitivity = 0.2f;
    fov = 45.0f;
    world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    up = vec3();
    right = vec3();

    updateCameraVectors();
}

void Camera::processKeyboard(Camera_Movement dir, float delta_time) {
    vec3 direction = vec3();
    switch (dir) {
    case FORWARD:
        direction = forward;
        break;
    case BACKWARD:
        direction = -forward;
        break;
    case RIGHT:
        direction = right;
        break;
    case LEFT:
        direction = -right;
        break;
    case UP:
        direction += world_up;
        break;
    case DOWN:
        direction -= world_up;
        break;
    default:
        break;
    }

    position += normalize(direction) * movement_speed * delta_time;
}

void Camera::processMouseScroll(float dy) {
    fov -= (float)dy;
    fov = (fov < 1.0f) ? 1.0f : (fov > 45.0f) ? 45.0f : fov;
}

void Camera::processMouseMovement(float dx, float dy) {
    dx *= sensitivity;
    dy *= sensitivity;

    yaw += dx;
    pitch += dy;
    pitch = (pitch > 89.0f) ? 89.0f : (pitch < -89.0f) ? -89.0f : pitch;

    updateCameraVectors();
}

mat4 Camera::getViewMatrix() {
    return lookAt(position, position + forward, up);
}

void Camera::updateCameraVectors() {

    forward.x = cos(radians(yaw)) * cos(radians(pitch));
    forward.y = sin(radians(pitch));
    forward.z = sin(radians(yaw)) * cos(radians(pitch));
    forward = normalize(forward);

    right = normalize(cross(forward, world_up));
    up = normalize(cross(right, forward));
}
