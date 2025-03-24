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

void ImGuiRenderer::render(MeshViewer& mv, RenderSettings& settings)
{
    // Begin
    ImGui::Begin("Mesh Viewer Control Panel");

    // Define
    ImGui::Text("Info");
    ImGui::Text("%s", ("FPS " + std::to_string(Time::FRAMES_PER_SECOND)).c_str());
    ImGui::NewLine();

    //---------------------
    // List of Meshes
    //---------------------
    //std::vector<char> tmp(n_meshes, 0);
    for (int i = 0; i < 1; ++i) {
        ImGui::PushID(i);
        const std::string& name = "TEMP";
        bool isDeleted = false;

        // Mesh Name
        if (ImGui::Selectable(name.c_str())) {
            std::cout << " clicked!" << std::endl;
        }
        ImGui::SameLine();

        // Rightclick menu
        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup(("mesh_popup_" + std::to_string(i)).c_str());
        }

        if (ImGui::BeginPopup(("mesh_popup_" + std::to_string(i)).c_str()))
        {
            std::string str = "Mesh";
            ImGui::Text("%s", str.c_str());

            if (ImGui::MenuItem("Delete"))
            {
                // TODO
                isDeleted = true;
            }

            ImGui::EndPopup(); // Close the popup
        }

        // Dont continue if a mesh just got deleted, wait for the next frame
        if (isDeleted)
        {
            ImGui::PopID();
            break;
        }

        // Mesh Visibility
        if (ImGui::Checkbox("##checkbox", &settings.visible))
        {
            // Visibility changed
        }

        ImGui::PopID();
    }

    //---------------------
    // Right Click on Mesh
    //---------------------
    if (mv.picked.batch_index != 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(("mesh_popup_" + std::to_string(mv.picked.batch_index)).c_str());
    }
    if (ImGui::BeginPopup(("mesh_popup_" + std::to_string(mv.picked.batch_index)).c_str()))
    {
        int i = mv.picked.batch_index - 1;
        std::string str = "TEMP";
        ImGui::Text("%s", str.c_str());

        // Mesh Visibility
        ImGui::Checkbox("Show Cells", &settings.showCells);
        ImGui::Checkbox("Show Faces", &settings.showFaces);
        ImGui::Checkbox("Show Edges", &settings.showEdges);
        ImGui::Checkbox("Show Vertices", &settings.showVertices);

        // Material
        ImGui::NewLine();
        ImGui::SliderFloat("Vertex Size", &settings.pointSize, 1.f, 16.0f);
        ImGui::SliderFloat("Edge Width", &settings.lineWidth, 1.f, 16.0f);
        ImGui::SliderFloat("Cell Scale", &settings.cellScale, 0.0f, 1.0f);
        ImGui::SliderFloat("Polygon Outline Width", &settings.outlineWidth, 0.0f, 12.0f);
        ImGui::ColorEdit3("Polygon Outline Color", &settings.outlineColor[0]);
        ImGui::Checkbox("Use Override Color", &settings.useColorOverride);
        ImGui::ColorEdit3("Override Color", &settings.colorOverride[0]);
        ImGui::ColorEdit3("Ambient", &settings.light.ambient[0]);
        ImGui::ColorEdit3("Diffuse", &settings.light.diffuse[0]);
        ImGui::ColorEdit3("Specular", &settings.light.specular[0]);

        ImGui::EndPopup();
    }

    //---------------------
    // Load Tet Mesh
    //---------------------
    if (ImGui::Button("Load Tet Mesh")) {
        IGFD::FileDialogConfig config;
        config.path = "..";
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ovm,.ovmb", config);
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filepath = ImGuiFileDialog::Instance()->GetFilePathName();

            MV::TetrahedralMesh mesh;
            MV::readMesh(filepath, mesh);
            //MV::TetMeshRenderer tetRenderer(mesh, mv.camera);
            mv.renderer.addTetMesh(mesh);
            //mv.addTetMesh(filepath, tetRenderer);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    //---------------------
    // General Settings
    //---------------------
    ImGui::Checkbox("Orthographic", &mv.camera.isOrthographic);
    ImGui::ColorEdit3("Background", &mv.clearColor[0]);

    //---------------------
    // Geometry Objects
    //---------------------
    ImGui::Separator();

    char buffer[128] = "";
    ImGui::InputText("(1)", buffer, sizeof(buffer));

    ImGui::Separator();

    // End
    ImGui::End();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}
