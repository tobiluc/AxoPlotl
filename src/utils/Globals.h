#ifndef GLOBALS_H
#define GLOBALS_H

//#include "Utils.h"
#include "GLFW/glfw3.h"
#include "Typedefs.h"
#include <array>

namespace MV
{

namespace Time
{
extern float DELTA_TIME;
extern float FRAMES_PER_SECOND;
extern float SECONDS_SINCE_LAST_FRAME;
extern float TIME_OF_LAST_FRAME;

// Updates Time variables
void update();

}

constexpr Color RED = {1, 0, 0};
constexpr Color GREEN = {0, 1, 0};
constexpr Color YELLOW = {1, 1, 0};
constexpr Color BLUE = {0, 0, 1};
constexpr Color PURPLE = {1, 0, 1};
constexpr Color LIGHT_BLUE = {0, 1, 1};
constexpr Color WHITE = {1, 1, 1};
constexpr Color BLACK = {0, 0, 0};
constexpr std::array<Color, 6> COLORS = {GREEN, BLUE, RED, YELLOW, PURPLE, LIGHT_BLUE};

extern Color CLEAR_COLOR;

class Camera;
extern Camera CAMERA;
extern bool IMGUI_FOCUS;
extern GLFWwindow* WINDOW;
extern float VIEWPORT_SIZE[2];

namespace Callbacks
{
extern float LAST_MOUSE_POSITION[2];
extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern void scroll_callback(GLFWwindow* window, double dx, double dy);
extern void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y);
}

}

#endif // GLOBALS_H
