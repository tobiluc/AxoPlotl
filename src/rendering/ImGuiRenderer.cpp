#include "ImGuiRenderer.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>
#include "../utils/Globals.h"
#include "../utils/FileAccessor.h"
#include "../commons/Camera.h"

namespace MV
{

void ImGuiRenderer::init(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiRenderer::newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    IMGUI_FOCUS = (io.WantCaptureMouse || io.WantCaptureKeyboard);
}

void ImGuiRenderer::render(MeshRenderSettings& settings)
{
    // Begin
    ImGui::Begin("Mesh Viewer Control Panel");

    // Define
    ImGui::Text("Info");
    ImGui::Text("%s", ("FPS " + std::to_string(Time::FRAMES_PER_SECOND)).c_str());
    ImGui::NewLine();

    if (ImGui::Button("Load Tet Mesh")) {
        IGFD::FileDialogConfig config;
        config.path = "..";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ovm,.ovmb", config);
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();

            MV::TetrahedralMesh mesh2;
            MV::readTetMesh(filepath, mesh2);
            //tetRenderer.setMesh(mesh2);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    ImGui::NewLine();
    ImGui::Text("Mesh Visibility");
    ImGui::Checkbox("Show Cells", &settings.showCells);
    ImGui::Checkbox("Show Faces", &settings.showFaces);
    ImGui::Checkbox("Show Edges", &settings.showEdges);
    ImGui::Checkbox("Show Vertices", &settings.showVertices);
    //ImGui::ColorEdit3("Background Color", &CLEAR_COLOR[0]);
    ImGui::NewLine();
    ImGui::Text("Mesh Render Settings");
    ImGui::SliderFloat("Vertex Size", &settings.pointSize, 1.f, 16.0f);
    ImGui::SliderFloat("Edge Width", &settings.lineWidth, 1.f, 16.0f);
    ImGui::SliderFloat("Cell Scale", &settings.cellScale, 0.0f, 1.0f);
    ImGui::SliderFloat("Polygon Outline Width", &settings.outlineWidth, 0.0f, 12.0f);
    ImGui::ColorEdit3("Polygon Outline Color", &settings.outlineColor[0]);
    ImGui::Checkbox("Use Override Color", &settings.useColorOverride);
    ImGui::ColorEdit3("Override Color", &settings.colorOverride[0]);
    ImGui::NewLine();
    ImGui::Text("Camera and Lighting");
    //ImGui::SliderFloat("fov", &MV::CAMERA.fov, 1.0f, 90.0f);
    ImGui::ColorEdit3("Ambient", &settings.light.ambient[0]);
    ImGui::ColorEdit3("Diffuse", &settings.light.diffuse[0]);
    ImGui::ColorEdit3("Specular", &settings.light.specular[0]);

    // End
    ImGui::End();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRenderer::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
}
