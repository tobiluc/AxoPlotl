#include "ImGuiRenderer.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

namespace AxoPlotl::GL
{

bool IMGUI_FOCUS = false;

void ImGuiInit(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    IMGUI_FOCUS = (io.WantCaptureMouse || io.WantCaptureKeyboard);
}

}

bool ImGui::InputFloat4x4(const std::string& label, glm::mat4 *M)
{
    ImGui::Text("%s", label.c_str());
    float rows[4][4];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            rows[i][j] = (*M)[j][i];
        }
    }
    bool res = false;
    res |= ImGui::InputFloat4("##Row1", rows[0]);
    res |= ImGui::InputFloat4("##Row2", rows[1]);
    res |= ImGui::InputFloat4("##Row3", rows[2]);
    res |= ImGui::InputFloat4("##Row4", rows[3]);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            (*M)[j][i] = rows[i][j];
        }
    }
    return res;
}
