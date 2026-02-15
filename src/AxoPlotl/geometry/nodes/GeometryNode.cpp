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
        ImGui::SeparatorText("Info");
        ImGui::Text("id=%d", id_);
        ImGui::SameLine();
        ImGui::InputText("Name", name_, sizeof(name_));

        ImGui::SeparatorText("Visualization Options");

        if (ImGui::Button("Zoom to Object")) {
            this->visible() = true;
            const auto& bbox = getBBox();
            scene->cameraSet().zoomToBox(bbox.first, bbox.second);
        }

        // Transform
        if (ImGui::BeginMenu("Transform")) {
            ImGui::InputFloat4x4("Matrix:", &transform_);
            if (ImGui::Button("Reset")) {transform_ = glm::mat4(1.0);} // Reset Transform
            ImGui::EndMenu();
        }

        // Material

        if (vol_rend_.n_vertices_ > 0) {
            if (ImGui::BeginMenu("Vertices")) {
                ImGui::Checkbox("Show Vertices", &vol_rend_.render_vertices_);
                ImGui::SliderFloat("Size", &vol_rend_.point_size_, 1.f, 64.0f);
                ImGui::EndMenu();
            }
        }

        if (vol_rend_.n_edges_ > 0) {
            if (ImGui::BeginMenu("Edges")) {
                ImGui::Checkbox("Show Edges", &vol_rend_.render_edges_);
                ImGui::SliderFloat("Width", &vol_rend_.line_width_, 1.f, 64.0f);
                ImGui::EndMenu();
            }
        }

        if (vol_rend_.n_faces_ > 0) {
            if (ImGui::BeginMenu("Faces")) {
                ImGui::Checkbox("Show Faces", &vol_rend_.render_faces_);
                ImGui::EndMenu();
            }
        }

        if (vol_rend_.n_cells_ > 0) {
            if (ImGui::BeginMenu("Cells")) {
                ImGui::Checkbox("Show Cells", &vol_rend_.render_cells_);
                ImGui::SliderFloat("Cell Scale", &vol_rend_.cell_scale_, 0.0f, 1.0f);
                ImGui::ColorEdit4("Outline Color", &vol_rend_.cell_outline_color_[0]);
                ImGui::EndMenu();
            }
        }

        if (ImGui::Checkbox("Enable Clip Box", &vol_rend_.clip_box_.enabled_)) {
            // Initially, we set the clip box to the entire bounding box, so everthing is visible
            vol_rend_.clip_box_.x_ = {bbox_.first[0], bbox_.second[0]};
            vol_rend_.clip_box_.y_ = {bbox_.first[1], bbox_.second[1]};
            vol_rend_.clip_box_.z_ = {bbox_.first[2], bbox_.second[2]};
        }
        if (vol_rend_.clip_box_.enabled_) {
            ImGui::SliderFloat2("x", &vol_rend_.clip_box_.x_[0], bbox_.first[0], bbox_.second[0]);
            ImGui::SliderFloat2("y", &vol_rend_.clip_box_.y_[0], bbox_.first[1], bbox_.second[1]);
            ImGui::SliderFloat2("z", &vol_rend_.clip_box_.z_[0], bbox_.first[2], bbox_.second[2]);
        }

        ImGui::SeparatorText("Edit");

        // Delete
        if (ImGui::MenuItem("Delete")) {
            deleted_ = true;
        }

        ImGui::EndPopup(); // Close the popup
    }
}

}
