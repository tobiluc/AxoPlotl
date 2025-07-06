#include "../rendering/ImGuiRenderer.h"
#include "MouseHandler.h"

namespace AxoPlotl
{

float MouseHandler::POSITION[2] = {0,0};
float MouseHandler::POSITION_DELTA[2] = {0,0};
float MouseHandler::SCROLL_DELTA[2] = {0,0};
bool MouseHandler::LEFT_PRESSED = false;
bool MouseHandler::RIGHT_PRESSED = false;
bool MouseHandler::LEFT_JUST_PRESSED = false;
bool MouseHandler::RIGHT_JUST_PRESSED = false;
bool MouseHandler::LEFT_JUST_RELEASED = false;
bool MouseHandler::RIGHT_JUST_RELEASED = false;

void MouseHandler::update(GLFWwindow* window)
{
    MouseHandler::LEFT_JUST_PRESSED = !MouseHandler::LEFT_PRESSED && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    MouseHandler::LEFT_PRESSED = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    MouseHandler::LEFT_JUST_RELEASED = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE);
    MouseHandler::RIGHT_JUST_PRESSED = !MouseHandler::RIGHT_PRESSED && (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    MouseHandler::RIGHT_PRESSED = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    MouseHandler::RIGHT_JUST_RELEASED = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE);

    //MouseHandler::POSITION_DELTA[0] = 0;
    //MouseHandler::POSITION_DELTA[1] = 0;
    //MouseHandler::SCROLL_DELTA[0] = 0;
    //MouseHandler::SCROLL_DELTA[1] = 0;
}

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y)
{
    if (Rendering::IMGUI_FOCUS) return;

    MouseHandler::POSITION_DELTA[0] = (float)(mouse_x - MouseHandler::POSITION[0]);
    MouseHandler::POSITION_DELTA[1] = (float)(MouseHandler::POSITION[1] - mouse_y); // reversed since y-coordinates range from bottom to top
    MouseHandler::POSITION[0] = (float)mouse_x;
    MouseHandler::POSITION[1] = (float)mouse_y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) return; // Rotate by dragging

    //CAMERA.processMouseMovement(MouseHandler::POSITION_DELTA[0], MouseHandler::POSITION_DELTA[1]);
}

void scroll_callback(GLFWwindow* window, double dx, double dy)
{
    if (Rendering::IMGUI_FOCUS) return;
    MouseHandler::SCROLL_DELTA[0] = dx;
    MouseHandler::SCROLL_DELTA[1] = dy;
    //MV::CAMERA.processMouseScroll((float)dy);
}

}
