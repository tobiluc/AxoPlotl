#include "Globals.h"
#include "GLFW/glfw3.h"

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
//float VIEWPORT_SIZE[2] = {800, 600};
//Color CLEAR_COLOR = {1, 1, 1};
//Camera CAMERA = Camera(glm::vec3(0.0f, 0.0f, 30.0f), glm::vec3(0.0f, 0.0f, -1.0f));
//GLFWwindow* WINDOW = nullptr;

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// void Callbacks::framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     VIEWPORT_SIZE[0] = (float)width;
//     VIEWPORT_SIZE[1] = (float)height;
//     glViewport(0, 0, width, height);
// }

}
