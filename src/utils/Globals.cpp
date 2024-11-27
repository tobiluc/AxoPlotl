#include "Globals.h"
#include "../commons/Camera.h"

namespace MV
{

float Time::DELTA_TIME = 0;
float Time::FRAMES_PER_SECOND = 0;
float Time::SECONDS_SINCE_LAST_FRAME = 0;
float Time::TIME_OF_LAST_FRAME = 0;

void Time::update()
{
    // Compute frames per second
    float TIME_OF_CURRENT_FRAME = (float)glfwGetTime();
    DELTA_TIME = TIME_OF_CURRENT_FRAME - TIME_OF_LAST_FRAME;
    TIME_OF_LAST_FRAME = TIME_OF_CURRENT_FRAME;
    SECONDS_SINCE_LAST_FRAME += DELTA_TIME;
    if (SECONDS_SINCE_LAST_FRAME >= 1.0f) {
        SECONDS_SINCE_LAST_FRAME -= 1.0f;
        FRAMES_PER_SECOND = 1.0f / DELTA_TIME;
    }
}

bool IMGUI_FOCUS = false;
float VIEWPORT_SIZE[2] = {800, 600};
Color CLEAR_COLOR = {1, 1, 1};
Camera CAMERA = Camera(glm::vec3(0.0f, 0.0f, 30.0f), glm::vec3(0.0f, 0.0f, -1.0f));
GLFWwindow* WINDOW = nullptr;

float Callbacks::LAST_MOUSE_POSITION[2] = {0, 0};

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Callbacks::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    VIEWPORT_SIZE[0] = (float)width;
    VIEWPORT_SIZE[1] = (float)height;
    glViewport(0, 0, width, height);
}

void Callbacks::scroll_callback(GLFWwindow* window, double dx, double dy) {
    if (IMGUI_FOCUS) return;
    MV::CAMERA.processMouseScroll((float)dy);
}

void Callbacks::mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y)
{
    if (IMGUI_FOCUS) return;

    float dx = (float)(mouse_x - LAST_MOUSE_POSITION[0]);
    float dy = (float)(LAST_MOUSE_POSITION[1] - mouse_y); // reversed since y-coordinates range from bottom to top
    LAST_MOUSE_POSITION[0] = (float)mouse_x;
    LAST_MOUSE_POSITION[1] = (float)mouse_y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) return; // Rotate by dragging

    MV::CAMERA.processMouseMovement(dx, dy);
}

}
