#ifndef IMGUIRENDERER_H
#define IMGUIRENDERER_H

#include "TetMeshRenderer.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>

namespace MV
{

class ImGuiRenderer
{
public:
    static void init();

    static void newFrame();

    static void render(TetMeshRenderer& tetRenderer);

    inline static void cleanup()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

private:
};
}
#endif // IMGUIRENDERER_H
