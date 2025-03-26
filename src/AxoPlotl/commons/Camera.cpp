#include "../rendering/ImGuiRenderer.h"
#include "Camera.h"
#include "../utils/Time.h"
#include "../utils/MouseHandler.h"

namespace AxPl
{

Camera::Camera(glm::vec3 position, glm::vec3 forward)
{
    perspective = PerspectiveProjection{
        .position = position,
        .forward = forward,
        .up = glm::vec3(0,0,0),
        .right = glm::vec3(0,0,0),
        .yaw = atan2(forward.x, forward.z),
        .pitch = 0,
        .fov = 0.25*M_PI
    };

    orthographic = OrthographicProjection{
        .position = glm::vec3(position.x, position.y, 100),
        .height = 10
    };

    updateCameraVectors();
}

void Camera::update(GLFWwindow* window)
{
    if (ImGuiRenderer::IMGUI_FOCUS) return;

    processKeyboard(window);
    processMouseScroll(MouseHandler::SCROLL_DELTA[1]);
    if (MouseHandler::LEFT_PRESSED)
    {
        processMouseMovement(MouseHandler::POSITION_DELTA[0], MouseHandler::POSITION_DELTA[1]);
    }
}

void Camera::processKeyboard(GLFWwindow *window)
{
    auto direction = glm::vec3(0,0,0);
    if (isOrthographic)
    {
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) direction.x += 1;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) direction.x -= 1;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) direction.y += 1;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) direction.y -= 1;
        if (!direction.x && !direction.y) return;
        orthographic.position += normalize(direction) * movement_speed * Time::DELTA_TIME;
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) processKeyboard(FORWARD);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) processKeyboard(BACKWARD);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) processKeyboard(LEFT);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) processKeyboard(RIGHT);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) processKeyboard(UP);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) processKeyboard(DOWN);
    }
}

void Camera::processKeyboard(CameraMovement dir) {
    glm::vec3 direction = glm::vec3(0,0,0);

    if (isOrthographic)
    {
        switch (dir) {
        case RIGHT: direction.x += 1; break;
        case LEFT: direction.x -= 1; break;
        case UP: direction.y += 1; break;
        case DOWN: direction.y -= 1; break;
        default: return; break;
        }
        orthographic.position += normalize(direction) * movement_speed * Time::DELTA_TIME;
    }
    else
    {
        switch (dir) {
        case FORWARD:
            direction = perspective.forward;
            break;
        case BACKWARD:
            direction = -perspective.forward;
            break;
        case RIGHT:
            direction = perspective.right;
            break;
        case LEFT:
            direction = -perspective.right;
            break;
        case UP:
            direction += world_up;
            break;
        case DOWN:
            direction -= world_up;
            break;
        default: return; break;
        }

        perspective.position += normalize(direction) * movement_speed * Time::DELTA_TIME;
    }
}

void Camera::processMouseScroll(float dy)
{
    if (isOrthographic)
    {
        orthographic.height = std::clamp(orthographic.height * (1-dy*sensitivity), 0.01f, 1000.f);
    }
    else
    {
        perspective.fov = std::clamp(perspective.fov - dy*sensitivity, 0.01f, (float)(0.5*M_PI));
    }
}

void Camera::processMouseMovement(float dx, float dy)
{
    //if (MV::ImGuiRenderer::isFocus()) return;

    if (!isOrthographic)
    {
        dx *= sensitivity;
        dy *= sensitivity;

        perspective.yaw += dx;
        perspective.pitch += dy;
        perspective.pitch = std::clamp(perspective.pitch, -1.5f, 1.5f); // between -89 and 89 degrees
    }

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    if (isOrthographic)
    {
        return lookAt(orthographic.position, orthographic.position + glm::vec3(0,0,-1), world_up);
    }
    else
    {
        return lookAt(perspective.position, perspective.position + perspective.forward, perspective.up);
    }
}

glm::mat4 Camera::getProjectionMatrix()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float width = viewport[2];
    float height = viewport[3];
    float aspect_ratio = width/height;
    return getProjectionMatrix(aspect_ratio);
}

glm::mat4 Camera::getProjectionMatrix(float width_over_height)
{
    constexpr float near = 0.1f;
    constexpr float far = 4096.0f;
    if (isOrthographic)
    {
        // Orthographic Projection
        const float ortho_height = orthographic.height;
        const float ortho_width = width_over_height * ortho_height;
        return glm::ortho(-0.5f*ortho_width, 0.5f*ortho_width, -0.5f*ortho_height, 0.5f*ortho_height, near, far);

    }
    else
    {
        // Perspective Projection
        return glm::perspective(perspective.fov, width_over_height, near, far);
    }
}

void Camera::updateCameraVectors()
{
    if (isOrthographic)
    {

    }
    else
    {
        perspective.forward.x = cos(perspective.yaw) * cos(perspective.pitch);
        perspective.forward.y = sin(perspective.pitch);
        perspective.forward.z = sin(perspective.yaw) * cos(perspective.pitch);
        perspective.forward = normalize(perspective.forward);

        perspective.right = normalize(cross(perspective.forward, world_up));
        perspective.up = normalize(cross(perspective.right, perspective.forward));
    }
}

}
