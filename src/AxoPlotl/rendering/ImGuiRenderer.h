#pragma once

#include "GLFW/glfw3.h"

namespace AxoPlotl::Rendering
{

extern bool IMGUI_FOCUS;

void ImGuiInit(GLFWwindow* window);

void ImGuiNewFrame();

}
