#include "GeometryNode.h"
#include "AxoPlotl/rendering/ImGuiRenderer.h"
#include <AxoPlotl/Scene.h>

namespace AxoPlotl
{

int GeometryNode::id_counter_ = 0;

void GeometryNode::renderUIHeader(Scene *scene)
{
    //---------------------
    // Draw colored circle
    //---------------------
    ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
    float circle_radius = 5.0f;
    ImVec2 circle_center = ImVec2(cursor_pos.x + circle_radius, cursor_pos.y + ImGui::GetTextLineHeight() * 0.5f);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddCircleFilled(circle_center, circle_radius, IM_COL32(255*ui_color_[0], 255*ui_color_[1], 255*ui_color_[2], 255));

    ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x + 2.0f * circle_radius + 4.0f, cursor_pos.y));

    bool deleted = false;

    //---------------------
    // Name
    //---------------------
    ImGui::Checkbox("", &visible());
    ImGui::SameLine();
    if (ImGui::Selectable(name_)) {
        // Open Popup with Settings
        ImGui::OpenPopup(("object_popup_" + std::to_string(id_)).c_str());
    }

    //---------------------
    // Options
    //---------------------
    ImGui::Checkbox("Expand", &show_ui_body_);

    //---------------------
    // Rightclick Menu
    //---------------------
    // if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    //     ImGui::OpenPopup(("object_popup_" + std::to_string(id_)).c_str());
    // }

    if (ImGui::BeginPopup(("object_popup_" + std::to_string(id_)).c_str())) {

        // Name
        ImGui::Text("Settings (Object Id: %d)", id_);
        ImGui::InputText("Name", name_, sizeof(name_));
        ImGui::Text("PLT: %d, %d, %d", mesh_renderer_.n_points(), mesh_renderer_.n_lines(), mesh_renderer_.n_triangles());
        ImGui::Separator();

        // if (ImGui::Button("Zoom to Object")) {
        //     const auto& bbox = getBBox();
        //     scene->cameraSet().zoomToBox(bbox.first, bbox.second);
        // }

        // Transform
        if (ImGui::BeginMenu("Transform")) {
            ImGui::InputFloat4x4("Matrix:", &transform_);
            if (ImGui::Button("Reset")) {transform_ = glm::mat4(1.0);} // Reset Transform
            ImGui::EndMenu();
        }

        // Material
        auto& settings = mesh_renderer_.settings();

        if (mesh_renderer_.n_points() > 0) {
            if (ImGui::BeginMenu("Vertices")) {
                ImGui::Checkbox("Show Vertices", &settings.renderPoints);
                ImGui::SliderFloat("Size", &settings.pointSize, 1.f, 32.0f);
                ImGui::EndMenu();
            }
        }

        if (mesh_renderer_.n_lines() > 0) {
            if (ImGui::BeginMenu("Edges")) {
                ImGui::Checkbox("Show Edges", &settings.renderLines);
                ImGui::SliderFloat("Width", &settings.lineWidth, 1.f, 16.0f);
                ImGui::EndMenu();
            }
        }

        if (mesh_renderer_.n_triangles() > 0) {
            if (ImGui::BeginMenu("Faces")) {
                ImGui::Checkbox("Show Faces", &settings.renderTriangles);
                ImGui::Checkbox("Wireframe", &settings.wireframe);
                // ImGui::Checkbox("Enable Face Color", &settings.useGlobalTriangleColor);
                // ImGui::ColorEdit3("Face Color", &settings.gobalTriangleColor[0]);
                // ImGui::SliderFloat("Outline Width", &settings.outlineWidth, 0.0f, 16.0f);
                // ImGui::ColorEdit3("Outline Color", &settings.outlineColor[0]);
                ImGui::EndMenu();
            }
        }

        // ImGui::ColorEdit3("Ambient", &settings.light.ambient[0]);
        // ImGui::ColorEdit3("Diffuse", &settings.light.diffuse[0]);
        // ImGui::ColorEdit3("Specular", &settings.light.specular[0]);
        ImGui::Separator();

        // Delete
        if (ImGui::MenuItem("Delete")) {
            deleted_ = true;
        }

        ImGui::EndPopup(); // Close the popup
    }
}

}
