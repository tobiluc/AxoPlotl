#include "../rendering/ImGuiRenderer.h"
#include "Camera.h"
#include "../utils/Time.h"
#include "../utils/MouseHandler.h"

namespace AxoPlotl
{

void PerspectiveCamera::update(GLFWwindow* window) {

    if (!GL::IMGUI_FOCUS)
    {
        // Compute Pitch/Yaw based on Input

        // Zoom
        float dy = MouseHandler::SCROLL_DELTA[1];
        if (dy) {
            dy *= sensitivity;
            orbit_distance = std::clamp(orbit_distance * (1.0f-dy), near, far);
        }

        // Pan
        if (MouseHandler::LEFT_PRESSED)
        {
            float dx = MouseHandler::POSITION_DELTA[0];
            float dy = MouseHandler::POSITION_DELTA[1];
            dx *= sensitivity;
            dy *= sensitivity;

            yaw -= dx;
            pitch = std::clamp(pitch - dy, -1.5f, 1.5f); // between -89 and 89 degrees
        }
    }

    // Update Vectors
    glm::vec3 offset;
    offset.x = orbit_distance * cos(pitch) * sin(yaw);
    offset.y = orbit_distance * sin(pitch);
    offset.z = orbit_distance * cos(pitch) * cos(yaw);

    position = orbit_target + offset;

    glm::vec3 forward = glm::normalize(-offset);
    glm::vec3 right = glm::normalize(glm::cross(forward, world_up));
    up = glm::normalize(glm::cross(right, forward));
}

void PerspectiveCamera::reset(GLFWwindow *window) {
    orbit_target = glm::vec3(0.0f);
    orbit_distance = 10.0f;
    fov = 0.25*M_PI;
    pitch = yaw = 0;
}

void OrthographicCamera::update(GLFWwindow* window) {
    if (GL::IMGUI_FOCUS) return;

    // Move around
    auto direction = glm::vec3(0,0,0);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) direction.x += 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) direction.x -= 1;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) direction.y += 1;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) direction.y -= 1;
    if (!direction.x && !direction.y) return;
    position += normalize(direction) * movement_speed * Time::DELTA_TIME;

    // Zoom
    float dy = MouseHandler::SCROLL_DELTA[1];
    if (dy) {
        dy *= sensitivity;
        height = std::clamp(height * (1.0f-dy), near, far);
    }
}

void OrthographicCamera::reset(GLFWwindow *window) {
    position = glm::vec3(0,0,1);
    height = 10;
}

void PerspectiveCamera::zoomToBox(const glm::vec3& min, const glm::vec3& max)
{
    orbit_target = 0.5f*(min+max);
    orbit_distance = glm::distance(max, min);
}

void OrthographicCamera::zoomToBox(const glm::vec3& min, const glm::vec3& max)
{
    position = 0.5f*(min+max);
    position[2] = 1;
    height = 1.5f*(max[1] - min[1]);
}

}
