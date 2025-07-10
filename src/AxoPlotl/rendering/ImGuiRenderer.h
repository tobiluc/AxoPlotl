#pragma once

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_float4x4.hpp"
#include <string>

namespace AxoPlotl::GL
{

extern bool IMGUI_FOCUS;

void ImGuiInit(GLFWwindow* window);

void ImGuiNewFrame();

}

namespace ImGui
{

bool InputFloat4x4(const std::string& label, glm::mat4* M);

}
