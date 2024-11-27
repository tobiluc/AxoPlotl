#include "Camera.h"
#include "../utils/Globals.h"

namespace MV
{

Camera::Camera(glm::vec3 position, glm::vec3 forward) {

    this->position = position;
    this->forward = forward;

    movement_speed = 40.0f;
    sensitivity = 0.2f;
    fov = 45.0f;
    world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    up = glm::vec3();
    right = glm::vec3();

    updateCameraVectors();
}

void Camera::processKeyboard()
{
    auto direction = glm::vec3();
    if (glfwGetKey(WINDOW, GLFW_KEY_W) == GLFW_PRESS) processKeyboard(FORWARD);
    if (glfwGetKey(WINDOW, GLFW_KEY_S) == GLFW_PRESS) processKeyboard(BACKWARD);
    if (glfwGetKey(WINDOW, GLFW_KEY_A) == GLFW_PRESS) processKeyboard(LEFT);
    if (glfwGetKey(WINDOW, GLFW_KEY_D) == GLFW_PRESS) processKeyboard(RIGHT);
    if (glfwGetKey(WINDOW, GLFW_KEY_SPACE) == GLFW_PRESS) processKeyboard(UP);
    if (glfwGetKey(WINDOW, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) processKeyboard(DOWN);
}

void Camera::processKeyboard(Camera_Movement dir) {
    glm::vec3 direction = glm::vec3();
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

    position += normalize(direction) * movement_speed * Time::DELTA_TIME;
}

void Camera::processMouseScroll(float dy) {
    fov -= (float)dy;
    fov = (fov < 1.0f) ? 1.0f : (fov > 90.0f) ? 90.0f : fov;
}

void Camera::processMouseMovement(float dx, float dy)
{
    //if (MV::ImGuiRenderer::isFocus()) return;

    dx *= sensitivity;
    dy *= sensitivity;

    yaw += dx;
    pitch += dy;
    pitch = (pitch > 89.0f) ? 89.0f : (pitch < -89.0f) ? -89.0f : pitch;

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return lookAt(position, position + forward, up);
}

glm::mat4 Camera::getProjectionMatrix()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float width = viewport[2];
    float height = viewport[3];
    float aspect_ratio = width/height;
    float near = 0.1f;
    float far = 4096.0f;
    return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
}

void Camera::updateCameraVectors() {

    forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward.y = sin(glm::radians(pitch));
    forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward = normalize(forward);

    right = normalize(cross(forward, world_up));
    up = normalize(cross(right, forward));
}

}
